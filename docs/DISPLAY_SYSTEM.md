# SYSTEM SPEC: Display Subsystem (STM32 + FreeRTOS + Abstract Display)

## 1. Architecture & Concurrency Model
- **RTOS Integration**: Uses FreeRTOS tasks, recursive mutexes, and binary semaphores.
- **Hardware Abstraction Layer (HAL)**: The system is strictly decoupled into two layers:
  1. **High-Level Manager (`display_manager.c/h`)**: Provides a universal, hardware-agnostic API for application logic and game code.
  2. **Low-Level Driver (`ssd1306.c/h`)**: Implements concrete hardware specifics (currently SSD1306 OLED $128 \times 64$ over I2C via DMA).
  * *Note on Portability*: To switch to a different display (e.g., SH1106), you only need to implement a new low-level driver adhering to the required drawing primitives, keeping the public `display_*` API completely untouched.
- **Thread Safety**: All public `display_*` API functions are thread-safe and wrapped in recursive mutexes (`display_lock`/`display_unlock`).
- **Async Refresh Loop (`display_task`)**:
  - Runs in a dedicated FreeRTOS task (`display_task`).
  - Waits for refresh triggers via binary semaphore (`need_update_take`) infinitely (`portMAX_DELAY`).
  - Automatically flushes the buffer to hardware when triggered, enforcing a minimum $50\,\text{ms}$ cool-down/delay between frames.
- **Hardware Layer (Current)**: Communicates with an SSD1306 OLED display ($128 \times 64$ pixels) over I2C using DMA (`I2C1_WriteDMA`, address `0x78`).

## 2. Initialization Workflow
1. Call `display_manager_init()` in `main()` to create synchronization primitives and spawn the display task.
2. Call `display_init()` to initialize the underlying hardware driver.

## 3. Core API Reference (`display_manager.h`)

### Properties & Control
- `uint16_t display_width(void)` -> Returns screen width (e.g., `128`)
- `uint16_t display_height(void)` -> Returns screen height (e.g., `64`)
- `void display_clear(void)` -> Clears local draw buffer.
- `void display_update(void)` -> Flushes draw buffer to hardware via DMA manually.

### Colors & Patterns
- `DISPLAY_COLOR_BG` (`0`) -> Background / Erase pixel.
- `DISPLAY_COLOR_FG` (`1`) -> Foreground / Draw pixel (lit).
- `DISPLAY_COLOR_NONE` (`255`) -> Transparent / Reserved.
- `display_pattern_t`: Enum of 12 built-in fill patterns (`DISPLAY_PAT_SOLID`, `DISPLAY_PAT_50`, `DISPLAY_PAT_GRID`, etc.).

### Drawing Primitives (Auto-triggers `need_update_set()`)
- `void display_pixel(int16_t x, int16_t y, color_t c)`
- `void display_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, color_t c)`
- `void display_fill_rect_pat(int16_t x, int16_t y, int16_t w, int16_t h, display_pattern_t pat, color_t c)`
- `void display_hline(int16_t x, int16_t y, int16_t w, color_t c)`
- `void display_vline(int16_t x, int16_t y, int16_t h, color_t c)`
- `void display_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, color_t c)`
- `void display_blit_mono(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmp, color_t c)`

### Advanced Synchronization (Atomic Multi-Primitive Operations)
If you need to draw a complex composite shape consisting of multiple primitives without intermediate screen tearing or refresh interruptions, wrap them explicitly:
```c
display_lock();
display_clear();
display_fill_rect(0, 0, 30, 30, DISPLAY_COLOR_FG);
display_line(0, 0, 30, 30, DISPLAY_COLOR_BG);
display_unlock();
need_update_set(); // Trigger refresh manually once done
```

## 4. Usage Rules
- **Never call low-level driver functions** (like `oled_*`) directly from application logic; always use the `display_*` abstraction layer. This ensures hardware interchangeability.
- **Coordinate system**: $(0,0)$ is top-left; max bounds are $x \in [0, \text{width}-1]$, $y \in [0, \text{height}-1]$.
- Drawing operations automatically request a screen refresh; manual calls to `display_update()` are rarely needed unless forcing an immediate sync.
