# Source provenance and attribution

This standalone target was extracted from the physical BACnet MS/TP work in the private CTRL Scout repository:

- Source repository: `lhumphries16/CTRL_Scout`
- Source path: `firmware/mega_mstp/`
- Extraction target: `lhumphries16/bacnet-mega-mstp`

The firmware is built against the official `bacnet-stack` source fetched by
`firmware/bootstrap_bacnet_stack.ps1`. The bootstrap script pins the checkout
to revision:

`6bfb0108d4d68835fd0c1062731b54f559cb1375`

No upstream `bacnet-stack` files are committed in this repository, and no
upstream pull request is part of this deliverable. The `port/` sources are
target-specific reductions/adaptations used to compile the Mega application
against the pinned stack.

The fetched upstream repository remains the authoritative source for its
copyright, license, and attribution notices. Do not remove those notices from
the local checkout when redistributing or packaging the build. Review the
upstream repository's current licensing terms for the pinned revision before
commercial redistribution.

CTRL Scout application code, the Python semantic runtime, the web UI, the .NET
MS/TP sidecar, recipes, and equipment profiles are intentionally excluded.
