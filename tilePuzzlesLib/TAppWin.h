#include <SDL.h>

#ifndef _TAPPWIN_H_
#define _TAPPWIN_H_

#include "GLogger.h"
#include "GameUtil.h"
#include "HexSpinRenderer.h"
#include "RollerRenderer.h"
#include "SliderRenderer.h"
#include "TRenderer.h"
#include "IRenderer.h"
#include "Tile.h"

#include "generated/resources/resources.h"
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
#include <filameshio/MeshReader.h>
#include <math/mat4.h>
#include <stb_image.h>
#include <utils/EntityManager.h>
#include <utils/Panic.h>
#include <utils/Path.h>

#include <functional>
#include <iostream>
#include <valarray>

using namespace filament;
using utils::Entity;
using utils::EntityManager;
using utils::Path;

namespace tilepuzzles {

static constexpr int WINDOW_WIDTH = 640;
static constexpr int WINDOW_HEIGHT = 480;
static constexpr int WINDOW_X_POS = 500;
static constexpr int WINDOW_Y_POS = 500;
static constexpr uint32_t WINDOW_FLAGS =
  SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;

struct TAppWin {

  void init() {
    ASSERT_POSTCONDITION(SDL_Init(SDL_INIT_EVENTS) == 0, "SDL_Init Failure");
    GameUtil::init();
    createRenderer();
    renderer->init();
    createWinow();
    setup_window();
    setup_animating_scene();
    game_loop();
    cleanup();
  }

  void createWinow() {
    auto title = std::string("Tile Puzzles");
    sdl_window = SDL_CreateWindow(title.c_str(), WINDOW_X_POS, WINDOW_Y_POS, WINDOW_WIDTH,
                                  WINDOW_HEIGHT, WINDOW_FLAGS);
  }

  // ---- cleanup ----
  void cleanup() {
    destroy_window();
    SDL_Quit();
  }

  /**
   * @brief
   *
   */
  void destroy_window() {
    renderer->destroy();
    SDL_DestroyWindow(sdl_window);
  }

  /**
   * @brief Set up animating scene
   *
   */
  void setup_animating_scene() {
    renderer->draw();
    needsDraw = true;
    onNewFrame = animation_new_frame;
  }

  /**
   * @brief Create a Renderer object
   *
   */
  void createRenderer() {
    // renderer = std::shared_ptr<IRenderer>(new SliderRenderer());
    // renderer = std::shared_ptr<IRenderer>(new RollerRenderer());
    renderer = std::shared_ptr<IRenderer>(new HexSpinRenderer());
  }

  /**
   * @brief
   *
   * @param win
   * @param dt
   */
  static void animation_new_frame(TAppWin& win, double dt) {
    win.renderer->update(dt);
    // win.renderer->animate(dt);
    win.needsDraw = true;
  }

  /**
   * @brief Set the up window object
   *
   */
  void setup_window() {
    void* nativeWindow = ::getNativeWindow(sdl_window);
    void* nativeSwapChain = nativeWindow;
#if defined(__APPLE__)
    void* metalLayer = nullptr;
    if (kBackend == filament::Engine::Backend::METAL) {
      metalLayer = setUpMetalLayer(nativeWindow);
      // The swap chain on Metal is a CAMetalLayer.
      nativeSwapChain = metalLayer;
    }
#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
    if (kBackend == filament::Engine::Backend::VULKAN) {
      // We request a Metal layer for rendering via MoltenVK.
      setUpMetalLayer(nativeWindow);
    }
#endif
#endif
    swapChain = renderer->createSwapChain(nativeSwapChain);
    resize_window();
  }

  /**
   * @brief
   *
   */
  void resize_window() {
#if defined(__APPLE__)
    void* nativeWindow = ::getNativeWindow(sdl_window);
    if (kBackend == filament::Engine::Backend::METAL) {
      resizeMetalLayer(nativeWindow);
    }
#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
    if (kBackend == filament::Engine::Backend::VULKAN) {
      resizeMetalLayer(nativeWindow);
    }
#endif
#endif

    int width, height;
    SDL_GL_GetDrawableSize(sdl_window, &width, &height);
    renderer->resize(width, height);
    needsDraw = true;
  }

  /**
   * @brief ---- event loop ----
   *
   */
  void game_loop() {
    size_t nClosed = 0;
    SDL_Event event;
    Uint64 lastTime = 0;
    const Uint64 kCounterFrequency = SDL_GetPerformanceFrequency();
    Uint32 totalFrameTicks = 0;
    Uint32 totalFrames = 0;
    bool buttonDown = false;

    while (nClosed < 1) {
      totalFrames++;
      Uint64 startTicks = SDL_GetPerformanceCounter();

      if (!UTILS_HAS_THREADING) {
        renderer->executeEngine();
      }
      while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
          case SDL_QUIT:
            nClosed = 1;
            break;

          case SDL_MOUSEMOTION:
            if (buttonDown) {
              filament::math::float2 pos = {float(event.motion.x), float(event.motion.y)};
              renderer->onMouseMove(pos);
            }
            break;

          case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
              case SDL_BUTTON_LEFT: {
                math::float2 mouseDownPos = {float(event.button.x),
                                             float(event.button.y)};
                renderer->onMouseDown(mouseDownPos);
                buttonDown = true;
                break;
              }

              case SDL_BUTTON_RIGHT:
                renderer->shuffle();
                break;
            }
            break;

          case SDL_MOUSEBUTTONUP:
            switch (event.button.button) {
              case SDL_BUTTON_LEFT: {
                math::float2 mouseUpPos = {float(event.button.x), float(event.button.y)};
                renderer->onMouseUp(mouseUpPos);
                buttonDown = false;
                break;
              }

              case SDL_BUTTON_RIGHT:
                break;
            }
            break;

          case SDL_WINDOWEVENT:
            switch (event.window.event) {
              case SDL_WINDOWEVENT_RESIZED:
                if (event.window.windowID == SDL_GetWindowID(sdl_window)) {
                  resize_window();
                  break;
                }
                break;
              case SDL_WINDOWEVENT_CLOSE:
                if (event.window.windowID == SDL_GetWindowID(sdl_window)) {
                  SDL_HideWindow(sdl_window);
                  break;
                }
                nClosed++;
                break;
              default:
                break;
            }
            break;
          default:
            break;
        }
      }
      Uint64 endTicks = SDL_GetPerformanceCounter();
      const double dt =
        lastTime > 0 ? (double(endTicks - lastTime) / kCounterFrequency) : (1.0 / 60.0);
      lastTime = endTicks;

      time += dt;
      if (onNewFrame) {
        onNewFrame(*this, time);
      }

      if (!needsDraw) {
        continue;
      }

      renderer->filaRender();
      needsDraw = false;
      lastDrawTime = time;
      Uint64 frameTicks = endTicks - startTicks;
      float frameTime = (float)frameTicks / (float)kCounterFrequency;
      float delay = floor(16.666f - frameTime);
      SDL_Delay(delay);
    }
  }

  std::function<void(TAppWin&, double)> onNewFrame;
  SDL_Window* sdl_window = nullptr;
  bool needsDraw = true;
  double time = 0.0;
  double lastDrawTime = 0.0;
  std::shared_ptr<IRenderer> renderer;
  SwapChain* swapChain = nullptr;
  Logger L;
};
} // namespace tilepuzzles
#endif