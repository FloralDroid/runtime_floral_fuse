# Floral Android FUSE integration

[简体中文](README.zh-CN.md)

The runtime supports `none` and traditional `lxcfs` modes. Explicit selection
never falls back to another backend; an omitted selection retains the existing
`/run/floral-lxcfs` source detection. `none` requires no FUSE support.

LXCFS proc, thermal, hwmon and framebuffer views retain their existing paths and
init mount behavior. `floral_fuse_bind --verify-only` checks the selected source;
it does not install a view or change the selection. The init builtin performs
mounts in the existing trusted init context.

Lupine native procfs/sysfs views are managed by the host and kernel. This runtime
has no Lupine backend, source handshake, policy audit or private event protocol.
The removed `lupine` FUSE mode is rejected. Android storage FUSE is unrelated.
