# Docker usage

Build the image

```
docker compose up -d --build
```

Run any command you want in the container

```
docker compose run ubuntu <command>
```

For example to build the kernel

```
docker compose run ubuntu make kernel
```

# qemu

First build the iso

```
docker compose run ubuntu make iso
```

Then launch the iso with qemu

```
qemu-system-x86_64 yoctos.iso
```
