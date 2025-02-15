<div align="center">

<img src="https://kanzakishigure.github.io/picx-images-hosting/LLKLiveAssist/download.9kgap50cti.webp" alt="LOGO" width="450" height="300" />

# LLKAssist

<br>
<div>
    <img alt="C++" src="https://img.shields.io/badge/c++-20-%2300599C?logo=cplusplus">
</div>
<div>
    <img alt="platform" src="https://img.shields.io/badge/platform-Windows%20-blueviolet">
    <img alt="license" src="https://img.shields.io/badge/license-MIT%20-orange">
</div>
<br>


LLKAssist -LULUCA小助手


一款基于[GPT-SoVITS](https://github.com/RVC-Boss/GPT-SoVITS)项目的弹幕姬

基于bili开放平台端口，实现自定义语音模型弹幕播报！

绝赞更新中  ✿✿ヽ(°▽°)ノ✿
<br>
</div>

## 亮点功能

- 朗读直播间弹幕信息，单屏直播更方便。
- 使用GPT-SoVITS作为模型后端，使用你自定义的声音模型进行弹幕播报。
- 可定义的舰长上舰及舰长进房特效。
<br>

## Quick start

### 下载最新release 包
下载最新release包后,仍然需要下载gpt-sovits整合包 [安装gpt-sovits](#安装gpt-sovits)

gpt-sovits整合包下载完成后请将对应文件解压到 目录 ***/GSoVITSCore/GPT-SoVITS-v2***中(请勿创建多级文件夹)

### 从源码构建
#### 获取源码
```git
git clone --recursive https://github.com/Liniyous/ElaWidgetTools.git
```
####  获取项目其他依赖

LLK Live Assist项目存在第三方依赖

+ boost
  
  boost源码进行构建 https://github.com/boostorg/boost
  并根据LLKAssitCore目录下的Cmakelist.txt设置boost库目录
+ qt5.15
  安装qt5.15 https://doc.qt.io/qt-5/gettingstarted.html


####  项目构建
项目使用cmake进行构建，支持msvc,Mingw编译器。
在项目根目录输入cmake
```cmake
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -USE_RCC=TRUE
cmake --build build --config Debug
```

### 安装gpt-sovits
+ gpt_sovits
  gpt-sovits整合包  
  > [huggingface(需魔法)：](https://huggingface.co/lj1995/GPT-SoVITS-windows-package/blob/main/GPT-SoVITS-v2-240821.7z)
  
  > [百度网盘(需超级会员可满速下载)：](https://pan.baidu.com/share/init?surl=OE5qL0KreO-ASHwm6Zl9gA&pwd=mqpi)

  > [iCloud(不限速,点击下载副本下载,如果下载时遇到错误，请退出登录)：](https://www.icloud.com/iclouddrive/0fejFggjP4AgBHXlxz2DLJ71w#GPT-SoVITS-v2-240821)
## Star History

<a href="https://star-history.com/#kanzakishigure/LLKLiveAssist&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=kanzakishigure/LLKLiveAssist&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=kanzakishigure/LLKLiveAssist&type=Date" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=kanzakishigure/LLKLiveAssist&type=Date" />
 </picture>
</a>



## 致谢

### 开源库

- 语音转换与语音合成：[GPT-SoVITS](https://github.com/RVC-Boss/GPT-SoVITS)
- c++ json序列化：[boost](https://github.com/boostorg/boost)
- c++ 网络库：[boost/beast](https://github.com/boostorg/beast)
- ssl加密：[openssl](https://github.com/openssl/openssl)
- 跨平台音频播放 [miniaudio](https://github.com/mackron/miniaudio)
- FluentUI风格的组件库 [ElaWidgetTools](https://github.com/Liniyous/ElaWidgetTools)