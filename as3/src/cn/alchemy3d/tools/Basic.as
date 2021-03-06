package cn.alchemy3d.tools
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.container.Scene3D;
	import cn.alchemy3d.view.Camera3D;
	import cn.alchemy3d.view.Viewport3D;
	
	import flash.events.Event;

	public class Basic extends Library
	{
		protected var viewport:Viewport3D;
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		
		public function Basic(viewWidth:int = 640, viewHeight:int = 480, fov:Number = 90, near:Number = 100, far:Number = 5000)
		{
			super();
			
			camera = new Camera3D(fov, near, far);
			camera.z = -100;
			
			scene = new Scene3D();
			
			viewport = new Viewport3D(this, viewWidth, viewHeight, camera, scene);
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
			viewport.render();
		}
	}
}