# V2Ray Desktop

[![Build Status](https://travis-ci.com/Dr-Incognito/V2Ray-Desktop.svg?branch=master)](https://travis-ci.com/Dr-Incognito/V2Ray-Desktop)
[![Build status](https://ci.appveyor.com/api/projects/status/0t07jpv22tf7xpn9?svg=true)](https://ci.appveyor.com/project/Dr-Incognito/V2Ray-Desktop)
[![LGTM grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/Dr-Incognito/V2Ray-Desktop.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/Dr-Incognito/V2Ray-Desktop/context:cpp)
[![LGTM alerts](https://img.shields.io/lgtm/alerts/g/Dr-Incognito/V2Ray-Desktop.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/Dr-Incognito/V2Ray-Desktop/alerts/)

## What's New in 2.0?

We use [Clash](https://github.com/Dreamacro/clash) as the backend proxy, which supports **Shadowsocks(R)**, **V2Ray**, and **Trojan** protocols.

## Introduction

V2Ray Desktop is a cross-platform GUI client that supports **Shadowsocks(R)**, **V2Ray**, and **Trojan** protocols, running on Windows, Linux, and macOS.
It is built with Qt 5 and QML 2.

Compared to [V2Ray](http://v2ray.com/), V2Ray Desktop provides more advanced features such as server subscription and latency test. You can easily migrate to V2Ray Desktop from [Shadowsocks-Qt5](https://github.com/shadowsocks/shadowsocks-qt5/) and [V2Ray Core](http://v2ray.com/) by importing their config files.

You can get the latest release at [Releases Page](https://github.com/Dr-Incognito/V2Ray-Desktop/releases).
If you are using Arch Linux, you can install from [archlinuxcn](https://github.com/archlinuxcn/mirrorlist-repo) or build from [AUR](https://aur.archlinux.org/packages/v2ray-desktop/).

**Notes:** 
- Some functions (*e.g.,* server subscription) in the prebuilt binary packages require **OpenSSL >= 1.1.0**. If you are using Ubuntu<=18.04, please install OpenSSL manually. You can refer to [Installation Guide](https://github.com/Dr-Incognito/V2Ray-Desktop/wiki/Installation) ([安装指南](https://github.com/Dr-Incognito/V2Ray-Desktop/wiki/安装指南)) in the Wiki page for the detailed information.
- The AppImage for Linux is built in Ubuntu 16.04. Linux with GLIBC< 2.23 (*e.g.,* Ubuntu<=16.04) may have problems using this AppImage. Please consider building it from source with Qt >= 5.15.

For more information, please visit the [project's Wiki page](https://github.com/Dr-Incognito/V2Ray-Desktop/wiki).

## Features

- Support Windows, Linux, and macOS.
- Support **Shadowsocks(R)**, **V2Ray**, and **Trojan** servers.
- Support connecting to multiple servers.
- Support adding/updating servers from subscription URLs.
- Support adding servers by importing [Shadowsocks-Qt5](https://github.com/shadowsocks/shadowsocks-qt5/) and [V2Ray Core](http://v2ray.com/) configuration.
- Support adding servers by scanning QR codes.
- Support PAC proxy mode, Global proxy mode, and Manual proxy mode.
- Support getting and setting system proxies for Windows, Linux (GNOME/KDE), and macOS.
- Support automatically updating GFWList.
- Support automatically starts up when logged in.

## Screenshot

<img width="1072" alt="Dashboard" src="https://user-images.githubusercontent.com/17879520/73626391-acae8500-4683-11ea-952f-b14dae450e0b.png">

<img width="1072" alt="Servers" src="https://user-images.githubusercontent.com/17879520/78095141-d9bfaf80-7408-11ea-83dc-b4106741676c.png">

## License

This project is licensed under version 3 of the GNU General Public License.
