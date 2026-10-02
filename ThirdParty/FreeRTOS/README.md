# FreeRTOS Kernel（手动移植，勿放入 Middlewares/）

- 版本：**FreeRTOS Kernel V11.1.0**（取自 `FreeRTOSv202406.05-LTS`）
- 目录结构（与工程 `CMakeLists.txt` 中的 `FREERTOS_DIR` 对应）：
  - `Source/*.c`：内核源文件（tasks/queue/list/timers/event_groups/stream_buffer/croutine）
  - `Source/include/`：内核头文件
  - `Source/portable/GCC/ARM_CM3/`：Cortex-M3 移植层（port.c / portmacro.h）
  - `Source/portable/MemMang/heap_4.c`：内存管理
- 配置头文件为 `Core/Inc/FreeRTOSConfig.h`（V11 风格，与内核版本匹配）

## 为什么放在 ThirdParty/ 而不是 Middlewares/

STM32CubeMX **每次重新生成代码都会清空 `Middlewares/` 目录**（本项目已因此丢失过一次内核）。
`ThirdParty/`、`User/` 属于用户自己的目录，CubeMX 不会触碰，所以内核必须放在这里。

## 注意事项

1. 不要在本工程的 `.ioc` 里启用 FreeRTOS 中间件，否则 CubeMX 会再带进来一套内核（V10.3.1 + CMSIS-RTOS2），造成两套内核冲突。
2. 不要创建与内核头文件仅“大小写不同”的文件名（如 `Core/Inc/freertos.h`），Windows 下会抢占 `FreeRTOS.h` 的解析。
