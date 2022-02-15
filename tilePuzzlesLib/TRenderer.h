#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "App.h"
#include "IOUtil.h"
#include "Mesh.h"
#include "Tile.h"
#include "IRenderer.h"

#include <filament/Camera.h>
#include <filament/Engine.h>
#include <filament/IndexBuffer.h>
#include <filament/IndirectLight.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/RenderableManager.h>
#include <filament/Renderer.h>
#include <filament/Scene.h>
#include <filament/Skybox.h>
#include <filament/Texture.h>
#include <filament/TextureSampler.h>
#include <filament/TransformManager.h>
#include <filament/VertexBuffer.h>
#include <filament/View.h>
#include <filament/Viewport.h>
#include <filamentapp/FilamentApp.h>
#include <filamentapp/IBL.h>
#include <filamentapp/NativeWindowHelper.h>
#include <math/mat4.h>
#include <stb_image.h>
#include <utils/EntityManager.h>
#include <utils/Panic.h>
#include <utils/Path.h>

using namespace filament;
using namespace filament::math;
using utils::Entity;
using utils::EntityManager;
using utils::Path;
using MinFilter = TextureSampler::MinFilter;
using MagFilter = TextureSampler::MagFilter;

namespace tilepuzzles {
template <typename VB, typename T>
struct TRenderer : IRenderer {
  TRenderer() {
  }

  virtual ~TRenderer() {
  }

  virtual T* onMouseDown(const float2& viewCoord) = 0;
  virtual T* onMouseUp(const float2& viewCoord) = 0;
  virtual void onMouseMove(const float2& dragPosition) = 0;
  virtual void initMesh() = 0;

  virtual void filaRender() {
    if (filaRenderer->beginFrame(swapChain)) {
      filaRenderer->render(view);
      filaRenderer->endFrame();
    }
  }

  virtual void resize(int width, int height) {
    view->setViewport({0, 0, uint32_t(width), uint32_t(height)});
    const float aspect = getAspectRatio();
    camera->setProjection(Camera::Projection::ORTHO, -aspect * zoom, aspect * zoom, -zoom,
                          zoom, kNearPlane, kFarPlane);
  }

  virtual void executeEngine() {
    if (engine) {
      engine->execute();
    }
  }

  virtual void init() {
    initMesh();
    engine = Engine::create();
    filaRenderer = engine->createRenderer();
    utils::EntityManager& em = utils::EntityManager::get();
    em.create(1, &cameraEntity);
    camera = engine->createCamera(cameraEntity);
    view = engine->createView();
    view->setCamera(camera);
    scene = engine->createScene();
    view->setScene(scene);
    app.camera = camera;
    app.view = view;
  }

  virtual void destroy() {
    if (mesh->hasBorder()) {
      engine->destroy(borderRenderable);
      engine->destroy(borderMatInstance);
      engine->destroy(borderTex);
    }

    engine->destroy(skybox);
    engine->destroy(renderable);
    engine->destroy(matInstance);
    engine->destroy(material);
    engine->destroy(vb);
    engine->destroy(ib);
    view->setScene(nullptr);
    engine->destroy(tex);
    engine->destroy(scene);
    engine->destroy(view);
    engine->destroyCameraComponent(cameraEntity);
    engine->destroy(cameraEntity);
    engine->destroy(swapChain);
    engine->destroy(filaRenderer);
    Engine::destroy(&engine);
  }

  float getAspectRatio() {
    return float(view->getViewport().width) / float(view->getViewport().height);
  }

  virtual SwapChain* createSwapChain(void* nativeSwapChain) {
    swapChain = engine->createSwapChain(nativeSwapChain);
    return swapChain;
  }

  virtual void update(double dt) {
    if (needsDraw) {
      needsDraw = false;
      vb->setBufferAt(
        *engine, 0,
        VertexBuffer::BufferDescriptor(mesh->vertexBuffer->cloneVertices(), mesh->vertexBuffer->getSize(),
                                       (VertexBuffer::BufferDescriptor::Callback)free));
      scene->remove(renderable);
      auto& rcm = engine->getRenderableManager();
      rcm.destroy(renderable);

      RenderableManager::Builder(1)
        .boundingBox({{-1, -1, -1}, {1, 1, 1}})
        .material(0, matInstance)
        .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, vb, ib, 0,
                  mesh->vertexBuffer->numIndices)
        .culling(false)
        .receiveShadows(false)
        .castShadows(false)
        .build(*engine, renderable);
      scene->addEntity(renderable);
    }
  }

  virtual void drawBorder() {
    if (mesh->hasBorder()) {
      std::shared_ptr<VB> vbBorder = mesh->vertexBufferBorder;
      Path path = FilamentApp::getRootAssetsPath() + "textures/border2.png";
      if (!path.exists()) {
        L.error("The texture ", path, " does not exist");
        return;
      }
      int w, h, n;
      unsigned char* data = stbi_load(path.c_str(), &w, &h, &n, 4);
      if (data == nullptr) {
        L.error("The texture ", path, " could not be loaded");
        return;
      }
      L.info("Loaded texture: y", w, "x", h);
      Texture::PixelBufferDescriptor buffer(
        data, size_t(w * h * 4), Texture::Format::RGBA, Texture::Type::UBYTE,
        (Texture::PixelBufferDescriptor::Callback)&stbi_image_free);

      static_assert(sizeof(Vertex) == (4 * 3) + (4 * 3) + (4 * 2),
                    "Strange vertex size.");
      borderTex = Texture::Builder()
                    .width(uint32_t(w))
                    .height(uint32_t(h))
                    .levels(1)
                    .sampler(Texture::Sampler::SAMPLER_2D)
                    .format(Texture::InternalFormat::RGBA8)
                    .build(*engine);
      borderTex->setImage(*engine, 0, std::move(buffer));
      TextureSampler sampler(MinFilter::LINEAR, MagFilter::LINEAR);
      // Create quad renderable
      borderVb =
        VertexBuffer::Builder()
          .vertexCount(vbBorder->numVertices)
          .bufferCount(1)
          .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3, 0,
                     32)
          .attribute(VertexAttribute::UV0, 0, VertexBuffer::AttributeType::FLOAT3, 24, 32)
          .build(*engine);
      borderVb->setBufferAt(*engine, 0,
                            VertexBuffer::BufferDescriptor(vbBorder->vertShapes,
                                                           vbBorder->getSize(), nullptr));
      borderIb = IndexBuffer::Builder()
                   .indexCount(vbBorder->numIndices)
                   .bufferType(IndexBuffer::IndexType::USHORT)
                   .build(*engine);
      borderIb->setBuffer(*engine, IndexBuffer::BufferDescriptor(vbBorder->indexShapes,

                                                                 vbBorder->getIndexSize(),
                                                                 nullptr));

      borderMatInstance = material->createInstance();
      borderMatInstance->setParameter("albedo", borderTex, sampler);
      borderRenderable = EntityManager::get().create();
      RenderableManager::Builder(1)
        .boundingBox({{-1, -1, -1}, {1, 1, 1}})
        .material(0, borderMatInstance)
        .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, borderVb, borderIb, 0,
                  vbBorder->numIndices)
        .culling(false)
        .receiveShadows(false)
        .castShadows(false)
        .build(*engine, borderRenderable);
      scene->addEntity(borderRenderable);
    }
  }

  virtual void draw() {
    drawTiles();
    drawBorder();
  }

  void drawTiles() {
    L.info("Using root asset path ", FilamentApp::getRootAssetsPath());
    Path path = FilamentApp::getRootAssetsPath() + "textures/1-30color.png";
    if (!path.exists()) {
      L.error("The texture ", path, " does not exist");
      return;
    }
    int w, h, n;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &n, 4);
    if (data == nullptr) {
      L.error("The texture ", path, " could not be loaded");
      return;
    }
    L.info("Loaded texture: y", w, "x", h);
    Texture::PixelBufferDescriptor buffer(
      data, size_t(w * h * 4), Texture::Format::RGBA, Texture::Type::UBYTE,
      (Texture::PixelBufferDescriptor::Callback)&stbi_image_free);

    static_assert(sizeof(Vertex) == (4 * 3) + (4 * 3) + (4 * 2), "Strange vertex size.");
    tex = Texture::Builder()
            .width(uint32_t(w))
            .height(uint32_t(h))
            .levels(1)
            .sampler(Texture::Sampler::SAMPLER_2D)
            .format(Texture::InternalFormat::RGBA8)
            .build(*engine);
    tex->setImage(*engine, 0, std::move(buffer));
    TextureSampler sampler(MinFilter::LINEAR, MagFilter::LINEAR);

    // Set up view
    skybox = Skybox::Builder().color({0., 0., 0., 0.0}).build(*engine);
    scene->setSkybox(skybox);

    view->setPostProcessingEnabled(false);
    view->setCamera(camera);

    // Create quad renderable
    vb =
      VertexBuffer::Builder()
        .vertexCount(mesh->vertexBuffer->numVertices)
        .bufferCount(1)
        .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3, 0,
                   32)
        .attribute(VertexAttribute::UV0, 0, VertexBuffer::AttributeType::FLOAT3, 24, 32)
        .build(*engine);
    vb->setBufferAt(*engine, 0,
                    VertexBuffer::BufferDescriptor(mesh->vertexBuffer->vertShapes,
                                                   mesh->vertexBuffer->getSize(),
                                                   nullptr));
    ib = IndexBuffer::Builder()
           .indexCount(mesh->vertexBuffer->numIndices)
           .bufferType(IndexBuffer::IndexType::USHORT)
           .build(*engine);
    ib->setBuffer(*engine, IndexBuffer::BufferDescriptor(
                             mesh->vertexBuffer->indexShapes,
                             mesh->vertexBuffer->getIndexSize(), nullptr));

    Path matPath = FilamentApp::getRootAssetsPath() + "textures/bakedTexture.filamat";
    std::vector<unsigned char> mat = IOUtil::loadBinaryAsset(matPath);
    material = Material::Builder().package(mat.data(), mat.size()).build(*engine);

    matInstance = material->createInstance();
    matInstance->setParameter("albedo", tex, sampler);
    renderable = EntityManager::get().create();
    RenderableManager::Builder(1)
      .boundingBox({{-1, -1, -1}, {1, 1, 1}})
      .material(0, matInstance)
      .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, vb, ib, 0,
                mesh->vertexBuffer->numIndices)
      .culling(false)
      .receiveShadows(false)
      .castShadows(false)
      .build(*engine, renderable);
    scene->addEntity(renderable);
    const float aspect = getAspectRatio();
    camera->setProjection(Camera::Projection::ORTHO, -aspect * zoom, aspect * zoom, -zoom,
                          zoom, kNearPlane, kFarPlane);
  }

  virtual void animate(double now) {
    auto& tcm = engine->getTransformManager();
    tcm.setTransform(
      tcm.getInstance(renderable),
      filament::math::mat4f::rotation(now, filament::math::float3{1, 0, 0}));
    tcm.setTransform(
      tcm.getInstance(borderRenderable),
      filament::math::mat4f::rotation(now, filament::math::float3{1, 0, 0}));
  }

  virtual void shuffle() {
    mesh->shuffle();
    needsDraw = true;
  }

  std::shared_ptr<Mesh<VB,T> > mesh;

  Logger L;
  Texture* tex;
  VertexBuffer* vb;
  IndexBuffer* ib;
  Skybox* skybox;
  Entity renderable;
  Engine* engine = nullptr;
  filament::Renderer* filaRenderer = nullptr;
  SwapChain* swapChain = nullptr;
  Entity cameraEntity;
  Camera* camera = nullptr;
  View* view = nullptr;
  Scene* scene = nullptr;
  Material* material = nullptr;
  MaterialInstance* matInstance = nullptr;

  VertexBuffer* borderVb;
  IndexBuffer* borderIb;
  Entity borderRenderable;
  MaterialInstance* borderMatInstance = nullptr;
  Texture* borderTex;

  App app;
  bool needsDraw = false;
  T* dragTile;

  static constexpr double kNearPlane = -1.;
  static constexpr double kFarPlane = 1.;
  static constexpr float zoom = 2.0f;
};

} // namespace tilepuzzles
#endif
