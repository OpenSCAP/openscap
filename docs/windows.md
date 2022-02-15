Windows Support Removal
===

We are removing Windows from supported platforms for OpenSCAP. This is not a revolutionary change in the project development process, but rather a formal declaration of the current state of things.

Meaning?
===

The core developers **will not**:

- Try to debug, resolve or fix any problems with the scanner functionality on Windows platform
- Produce release builds

The core developers **will**:

- Make sure that OpenSCAP is buildable on Windows for the entire lifetime of version 1.3.x

**The last somewhat working release for Windows is [1.3.4](releases/tag/1.3.4).**


But why?
===

Ever since the support was introduced (1.3) the code that covers it has been undermaintained.
The support was introduced in the hope that developers, who are interested in the Windows platform, would join the community and help properly maintain it.
It never happened.
The codebase is now degraded to the state when OpenSCAP is no longer usable on that platform.

It works for me, I’d like to continue using it!
===

It most likely means that you’ve managed to fix the code base and support all the changes that happen in the ecosystem of the platform.
This also means that you are able to do your own builds.
We won’t try to break the build system or the code in a way that it would be incompatible with Windows, so you can safely use the latest [1.3.4](releases/tag/1.3.4) version of the OpenSCAP for as long as you wish.
Moreover, if you are interested in supporting the Windows version officially (fixing and maintaining it on a quality level) we would be more than happy to have you as a part of our community. Just let us know.
