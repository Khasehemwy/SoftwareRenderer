# SoftwareRenderer3D

3D软件渲染器。  
**不借助外部库**，仅使用C++标准库而制作的3D软渲染器。  
  
(Windows平台需要使用WinAPI)  
(模型导入需要assimp库)

## 博客:

[软渲染器记录](https://blog.csdn.net/Khasehemwy/article/details/115186533)

## 功能:
 #### 光栅化:
 * 线框绘制(顶点着色器和几何着色器) - Wireframe drawing (Vertex Shader and Geometry shader)
 * 片段着色器(像素着色器) - Fragment Shader (Pixel Shader)
 * 高洛德着色 - Gouraud Shading

 #### 光线追踪:
 * Whitted-Style光线追踪 - Whitted-Style
 * PBR光线追踪 - Physically Based Rendering
 * 全局光照 - Global Illumination

 #### 光照:
 * 光照: (Lighting)
   * 高洛德着色 - Gouraud Shading
   * 冯氏着色 - Phong Shading
   * 环境光照 + 漫反射光照 + 镜面光照 - Ambient light + Diffuse light + Specular lighting  
   * 定向光源 + 点光源 + 聚光源 - Directional light + Point light + Spotlight
   * 多光源 - Multiple light source
 * 阴影 - Shadow

 #### 纹理:
 * 仿射/透视纹理映射 - Affine texture mapping (暂时只支持手动创建的纹理)
 * 双线性滤波 - Bilinear filtering

 #### 优化:
 * 背面剔除 - Back culling
 * 透视修正 - Perspective correction
 * 深度缓存(1/z) - Depth buffer(1/z-buffer)
 * 简单CVV裁剪 - Canonical View Volume Clip
 * 水平裁剪,垂直裁剪 - Horizontal Clip and Vertical Clip

 #### 外部导入
 * 外部3D模型导入

## 展示:
<image src="https://user-images.githubusercontent.com/57032017/114022261-2ce20300-98a4-11eb-8940-c42f300ea451.gif" width=50%>
<image src="https://user-images.githubusercontent.com/57032017/114022285-323f4d80-98a4-11eb-8c01-f21ad017de0d.gif" width=50%>
<image src="https://user-images.githubusercontent.com/57032017/114022350-3f5c3c80-98a4-11eb-8b6c-98abb6e14a1f.gif" width=50%>
<image src="https://user-images.githubusercontent.com/57032017/114022481-661a7300-98a4-11eb-8b3b-0b181aba7251.gif" width=50%>
<image src="https://user-images.githubusercontent.com/57032017/114022515-6e72ae00-98a4-11eb-82af-701380da90c6.gif" width=50%>
<image src="https://user-images.githubusercontent.com/57032017/114256731-0334f300-99ee-11eb-969d-69db3882f3a4.gif" width=50%>
<image src="https://user-images.githubusercontent.com/57032017/114263461-21afe400-9a18-11eb-95a8-cdb0b2a91f79.gif" width=50%>
<image src="https://user-images.githubusercontent.com/57032017/114267423-0c45b480-9a2e-11eb-92c2-e5833505c8de.png" width=50%>  
<image src="https://user-images.githubusercontent.com/57032017/114396714-6ed6b600-9bd0-11eb-91f8-dd6c897d0652.gif" width=50%>  
<image src="https://user-images.githubusercontent.com/57032017/114353745-f5bf6a80-9b9f-11eb-85ce-7937359b624b.png" width=50%>
<image src="https://user-images.githubusercontent.com/57032017/116544593-ed965780-a921-11eb-9d71-d6a45c07503e.png" width=50%>
<image src="https://user-images.githubusercontent.com/57032017/117296196-5b54fd00-aea7-11eb-8c2b-91893639804f.png" width=100%>

  
 <table><tr>
<td><image src="https://user-images.githubusercontent.com/57032017/134773833-c045a4cd-a5de-4204-aa4e-1ba45e155603.gif" width=100% height="100%" border=0><p>高洛德着色</p></td>
<td><image src="https://user-images.githubusercontent.com/57032017/134773844-8bb864a0-eda3-409c-a8d3-51ce31f79ac9.gif" width=100% height="100%" border=0><p>冯氏着色 (GIF左侧是帧率,很明显冯氏着色慢一些)</p></td>
</tr></table>

  
 <table><tr>
<td><image src="https://user-images.githubusercontent.com/57032017/134305940-d7cb79f6-65db-408f-869a-0f49800a1572.gif" width=100% height="100%" border=0><p>基于高洛德着色的阴影</p></td>
<td><image src="https://user-images.githubusercontent.com/57032017/134793343-4129b1fd-3a8e-40d1-9c77-0bfd17c05953.gif" width=100% height="100%" border=0><p>基于冯氏着色的阴影</p></td>
</tr></table>
  
  
<table><tr>
<td><image src="https://user-images.githubusercontent.com/57032017/140636655-addf4277-b206-4290-9112-c8ec715c28ea.png" width=100% height="100%" border=0><p>光栅化(无阴影)</p></td>
<td><image src="https://user-images.githubusercontent.com/57032017/140636674-e5f389b0-d8e7-4d75-912e-2dcd5abdfe05.png" width=100% height="100%" border=0><p>光追(Whitted-Style)</p></td>
<td><image src="https://user-images.githubusercontent.com/57032017/140636706-d338cb9d-b934-4f15-9740-d14985d4cc87.png" width=100% height="100%" border=0><p>光追(Whitted-Style),添加全局光照Trick(右侧方块有部分蓝色反光)</p></td>
<td><image src="https://user-images.githubusercontent.com/57032017/140640224-5b04d171-5bc7-4b44-9fce-a1965eabade5.png" width=100% height="100%" border=0><p>光追(PBR),4深度,4采样点</p></td>
<td><image src="https://user-images.githubusercontent.com/57032017/140640246-1d9fe998-5180-4f59-a9de-2bf6d23ac848.png" width=100% height="100%" border=0><p>光追(PBR),16深度,2048采样点</p></td>
</tr></table>
