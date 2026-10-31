# Floral Android FUSE 集成

[English](README.md)

项目位于 `system/floral/fuse`，支持 `none` 和传统 `lxcfs`。显式选择不自动切换后端；未指定时保留 `/run/floral-lxcfs` 来源探测。`none` 不依赖 FUSE。

传统 LXCFS 的 proc、thermal、hwmon、framebuffer 视图、路径及 init 内建挂载保持原行为。`floral_fuse_bind --verify-only` 只复核所选来源，不安装视图或重新选择模式；挂载仍由既有 init 内建命令执行。

Lupine 的原生 procfs/sysfs 视图由宿主和内核管理。本仓库不再提供 Lupine 后端、来源握手、策略审计或私有事件协议；旧的 `lupine` FUSE 模式明确拒绝。Android 存储 FUSE 是独立功能。
