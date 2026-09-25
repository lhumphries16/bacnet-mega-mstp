# Source provenance and attribution

This repository combines:

- local ATmega2560 platform code in src/;
- a small in-memory BACnet demonstration application;
- source files fetched unchanged from the pinned bacnet-stack checkout.

The upstream bacnet-stack source is not committed here. The bootstrap scripts
fetch it into vendor/bacnet-stack and verify BACNET_STACK_REVISION before a
build is allowed to proceed.

The upstream source carries its own copyright and license notices. The fetched
checkout is authoritative for those notices; preserve its license directory
when redistributing a build. The upstream stack includes components under more
than one license, so review the pinned checkout before commercial
redistribution.

The local code is intended as a platform reference and laboratory example. It
is not presented as an upstream bacnet-stack implementation.
