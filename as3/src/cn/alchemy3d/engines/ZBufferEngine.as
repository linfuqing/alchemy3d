package cn.alchemy3d.engines
{
	import cmodule.alchemy3d.CLibInit;
	
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.materials.MaterialsManager;
	import cn.alchemy3d.objects.Entity;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.Viewport3D;
	
	import flash.display.BitmapData;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class ZBufferEngine
	{
		protected var buffer:ByteArray;
		protected var alchemy3DLib:Object;
		
		public function ZBufferEngine()
		{
			var clib:CLibInit = new CLibInit();
			alchemy3DLib = clib.init();
			
			var ns:Namespace = new Namespace("cmodule.alchemy3d");
			buffer = (ns::gstate).ds;
			buffer.endian = Endian.LITTLE_ENDIAN;
		}
		
		public function renderScene(camera:Camera3D, viewport:Viewport3D, scene:Scene3D, materialsManager:MaterialsManager):void
		{
			//摄像机序列化
			var cameraSerialized:ByteArray = camera.serialize();
			//材质序列化
			var materialsSerialized:ByteArray = materialsManager.serialize();
			//几何体序列化
			var geomSerialized:ByteArray = new ByteArray();
			geomSerialized.endian = Endian.LITTLE_ENDIAN;
			var p:Entity, pSerialized:ByteArray;
			var children:Vector.<Entity> = scene.children;
			//遍历所有几何体
			for each (p in children)
			{
				pSerialized = p.serialize();
				geomSerialized.writeBytes(pSerialized, 0, pSerialized.length);
			}
			
			//初始化位图
			alchemy3DLib.initializeGfx(viewport.viewWidth, viewport.viewHeight);
			//初始化摄像机
			alchemy3DLib.initializeCamera(camera.zoom, camera.focus, camera.nearClip, camera.farClip, cameraSerialized.length);
			//初始化几何体
			alchemy3DLib.initializeGeom(scene.childrenNum, geomSerialized.length);
			//初始化材质
			alchemy3DLib.initializeMaterials(materialsSerialized.length);
			
			//获得摄像机缓冲区起始偏移量
			buffer.position = alchemy3DLib.getCameraBufferPointer();
			//序列化数据写入摄像机缓冲区
			buffer.writeBytes(cameraSerialized, 0, cameraSerialized.length);
			
			//获得材质缓冲区起始偏移量
			buffer.position = alchemy3DLib.getMaterialsBufferPointer();
			//序列化数据写入摄像机缓冲区
			buffer.writeBytes(materialsSerialized, 0, materialsSerialized.length);
			
			//获得几何体缓冲区起始偏移量
			buffer.position = alchemy3DLib.getGeomBufferPointer();
			//序列化数据写入摄像机缓冲区
			buffer.writeBytes(geomSerialized, 0, geomSerialized.length);
			
			//顶点变换
			alchemy3DLib.transformVertices();
			//面剔除
			alchemy3DLib.facesCulling();
			//光栅化
			alchemy3DLib.rasterize();
			
			//获得位图缓冲区起始偏移量
			buffer.position = alchemy3DLib.getGfxBufferPointer();
			//绘制
			var gfx:BitmapData = viewport.bitmapData;
			gfx.setPixels(gfx.rect, buffer);
		}
	}
}