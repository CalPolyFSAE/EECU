## RTOS Template Notes

The config file `source/FreeRTOSConfig.h` should work out of the box.

### Interrupts

Any interrupts need to have priority adjusted before use. For example:

```
NVIC->IP[x] |= 6 << 4;
```

... where `x` is the interrupt's IRQ number from table 4-2 of the KE1xF
reference.  `6` here is the priority, out of a maximum of 15. Note that a higher
number represents a lower priority. It is important that this value is greater
than `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` as defined in
`source/FreeRTOSConfig.h`
