package cn.alchemy3d.view
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.scene.Scene3D;
	
	import flash.events.Event;
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	import flash.utils.getTimer;

	public class Basic extends Library
	{
		protected var viewport:Viewport3D;
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		
		private var _timer:Timer;
		private var _frameRate:Number;
		
		public function Basic(frameRate:Number = 60, viewWidth:int = 640, viewHeight:int = 480, fov:Number = 90, near:Number = 100, far:Number = 5000)
		{
			super();
			
			_frameRate = frameRate;
			
			scene = new Scene3D();
			
			camera = new Camera3D(fov, near, far);
			camera.z = -100;
			
			viewport = new Viewport3D(viewWidth, viewHeight, scene, camera);
			addChild(viewport);
			
//			_timer = new Timer(1000/_frameRate, 0);
//			_timer.addEventListener(TimerEvent.TIMER, onRenderTick);
		}
		
		public function startRendering():void
		{
			addEventListener(Event.ENTER_FRAME, onRenderTick);
//			_timer.start();
		}
		
		public function stopRendering():void
		{
			removeEventListener(Event.ENTER_FRAME, onRenderTick);
//			_timer.stop();
		}
		
		protected function onRenderTick(e:Event = null):void
		{
			alchemy3DLib.render(viewport.pointer, getTimer());
			
			viewport.render();
		}
	}
}