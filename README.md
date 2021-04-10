# SoftwareRenderer3D

3D软件渲染器。
不借助外部库，仅使用C++标准库而制作的3D软渲染器。（Windows平台需要使用WinAPI）

## 博客:

[软渲染器记录](https://blog.csdn.net/Khasehemwy/article/details/115186533)

## 功能:
* 线框绘制(顶点着色器和几何着色器) - Wireframe drawing (Vertex Shader and Geometry shader)
* 背面剔除 - Back culling
* 片段着色器(像素着色器) - Fragment Shader (Pixel Shader)
* 高洛德着色 - Gouraud Shading
* 使用高洛德着色的局部光照：(Lighting,use Gouraud Shading)  
  * 环境光照 + 漫反射光照 + 镜面光照 - Ambient light + Diffuse light + Specular lighting  
  * 定向光源 + 点光源 + 聚光源 - Directional light + Point light + Spotlight
  * 多光源 - Multiple light source
* 仿射/透视纹理映射 - Affine texture mapping (暂时只支持手动创建的纹理)
* 透视修正 - Perspective correction
* 深度缓存(1/z) - Depth buffer(1/z-buffer)
* 简单CVV裁剪

## 展示:
<image src="https://user-images.githubusercontent.com/57032017/114022261-2ce20300-98a4-11eb-8940-c42f300ea451.gif" width=60%>
<image src="https://user-images.githubusercontent.com/57032017/114022285-323f4d80-98a4-11eb-8c01-f21ad017de0d.gif" width=60%>
<image src="https://user-images.githubusercontent.com/57032017/114022350-3f5c3c80-98a4-11eb-8b6c-98abb6e14a1f.gif" width=60%>
<image src="https://user-images.githubusercontent.com/57032017/114022481-661a7300-98a4-11eb-8b3b-0b181aba7251.gif" width=60%>
<image src="https://user-images.githubusercontent.com/57032017/114022515-6e72ae00-98a4-11eb-82af-701380da90c6.gif" width=60%>
<image src="https://user-images.githubusercontent.com/57032017/114256731-0334f300-99ee-11eb-969d-69db3882f3a4.gif" width=60%>
<image src="https://user-images.githubusercontent.com/57032017/114263461-21afe400-9a18-11eb-95a8-cdb0b2a91f79.gif" width=60%>
<image src="https://user-images.githubusercontent.com/57032017/114267423-0c45b480-9a2e-11eb-92c2-e5833505c8de.png" width=60%>


