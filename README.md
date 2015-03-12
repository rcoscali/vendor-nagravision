vendor-nagravision
==================

Drm &amp; Crypto plugins for ASPIRE demo

Warning: This is NOT a secure Crypto & Drm plugins implem. 
This code was developped as a demonstration Use Case for the ASPIRE European Project.

"The mission of the ASPIRE project is to integrate state-of-the-art software protection techniques into an application reference architecture and into an easy-to-use compiler framework that automatically provides measurable software-based protection of the valuable assets in the persistently or occasionally connected client applications of mobile service, software, and content providers."

https://aspire-fp7.eu/

This code is intended to be built in an Android Source tree (JB or KK) and shall be located in vendor/nagravision directory (with a link from build/target/product/full_base.mk makefile near libfwdlockengine -- by adding libnvcryptoplugin & libnvdrmplugin). If you don't know what I mean, don't try it. 
This file for KK: https://android.googlesource.com/platform/build/+/kitkat-mr2.1-release/target/product/full_base.mk

There is a build kit that avoid having a whole android source tree... I'll add it later
