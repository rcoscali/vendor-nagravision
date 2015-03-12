vendor-nagravision
==================

Drm &amp; Crypto plugins for ASPIRE demo

Warning: This is NOT a secure Crypto & Drm plugins implem. 
This code was developped as a demonstration Use Case for the ASPIRE Project.

"The mission of the ASPIRE project is to integrate state-of-the-art software protection techniques into an application reference architecture and into an easy-to-use compiler framework that automatically provides measurable software-based protection of the valuable assets in the persistently or occasionally connected client applications of mobile service, software, and content providers."

[Link to aspire Project Web Site](https://aspire-fp7.eu/)

This code is intended to be built in an Android Source tree (JB or KK) and shall be located in vendor/nagravision directory (with a link from build/target/product/full_base.mk makefile near libfwdlockengine -- by adding libnvcryptoplugin & libnvdrmplugin). If you don't know what I mean, don't try it. 
This file for KK: `https://android.googlesource.com/platform/build/+/kitkat-mr1-release/target/product/full_base.mk`
This way for ex:

```
========================================================================================
*** build/target/product/full_base.mk.orig	2015-01-20 08:57:29.481673521 +0100
--- build/target/product/full_base.mk	2014-10-10 09:45:01.734319095 +0200
***************
*** 21,26 ****
--- 21,28 ----
  
  PRODUCT_PACKAGES := \
      libfwdlockengine \
+     libnvdrmplugin \
+     libnvcryptoplugin \
      OpenWnn \
      PinyinIME \
      libWnnEngDic \
========================================================================================
```

Branches for which plugins were tested against are: kitkat-mr1-release jb-mr2.0-release
Branches for which plugins should run are: all JB and KK.
On later branches, media framework changed a lot... I didn't event tried.

There is a build kit that avoid having a whole android source tree... I'll add it later
