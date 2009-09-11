package cn.alchemy3d.view
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.scene.Scene3D;
	
	import flash.events.Event;
	import flash.utils.getTimer;

	public class Basic extends Library
	{
		protected var viewport:Viewport3D;
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		
		public function Basic(viewWidth:int = 640, viewHeight:int = 480, fov:Number = 90, near:Number = 100, far:Number = 5000)
		{
			super();
			
			scene = new Scene3D();
			
			camera = new Camera3D(fov, near, far);
			camera.z = -100;
			
			viewport = new Viewport3D(viewWidth, viewHeight, scene, camera);
			addChild(viewport);
		}
		
		public function startRendering():void
		{
			addEventListener(Event.ENTER_FRAME, onRenderTick);
		}
		
		public function stopRendering():void
		{
			removeEventListener(Event.ENTER_FRAME, onRenderTick);
		}
		
		protected function onRenderTick(e:Event = null):void
		{
			alchemy3DLib.render(viewport.pointer, getTimer());
			
			viewport.render();
		}
	}
}