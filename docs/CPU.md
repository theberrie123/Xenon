# CPU

## Global Descriptor Table (GDT)
The GDT is a data structure used by x86 CPUs to define the
characteristics of the various memory segments used in
protected mode. Each entry in the GDT describes a segment's
base address, size (limit), access permissions, and other
attributes. It allows the CPU to enforce memory protection and
privilege levls by controlling which parts of memory code and
data segments can access. It is essential for switching from
real mode to protected mode and for defining kernel and user
space segments in an OS.
