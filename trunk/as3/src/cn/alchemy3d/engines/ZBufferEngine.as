package cn.alchemy3d.engines
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.materials.MaterialsManager;
	import cn.alchemy3d.polygon.DisplayObject3D;
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
			var cameraSerialized:ByteArray = camera.serialize();
			var materialsSerialized:ByteArray = materialsManager.serialize();
			var geomSerialized:ByteArray = new ByteArray();
			geomSerialized.endian = Endian.LITTLE_ENDIAN;
			var p:DisplayObject3D, pSerialized:ByteArray， children:Vector.<DisplayObject3D> = scene.children;
			for each (p in children)
			{
				pSerialized = p.serialize();
				geomSerialized.writeBytes(pSerialized, 0, pSerialized.length);
			}
			
			alchemy3DLib.initializeViewport(viewport.viewWidth, viewport.viewHeight);
			alchemy3DLib.initializeCamera();
			alchemy3DLib.initializeMaterials();
			
			buffer.position = alchemy3DLib.getCameraBufferPointer();
			buffer.writeBytes(cameraSerialized, 0, cameraSerialized.length);
			
			buffer.position = alchemy3DLib.getMaterialsBufferPointer();
			buffer.writeBytes(materialsSerialized, 0, materialsSerialized.length);
			
			buffer.position = alchemy3DLib.getGeomBufferPointer();
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