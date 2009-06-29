package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.View3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.KeyboardEvent;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class Alchemy3D extends Sprite
	{
		private var viewport:View3D;
		private var scene:Scene3D;
		private var camera:Camera3D;
		
		protected var lib:Alchemy3DLib;
		
		protected var p:Plane;
		protected var p2:Plane;
		
		public function Alchemy3D()
		{
			super();
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.quality = StageQuality.HIGH;
			stage.frameRate = 60;
			
			scene = new Scene3D();
			camera = new Camera3D(0, 90, 100, 5000);
			viewport = new View3D(640, 480, scene, camera);
			
			addChild(viewport);
			
			var fps:FPS = new FPS(scene);
			addChild(fps);
			
			lib = Alchemy3DLib.getInstance();
			
			p = new Plane(1, 200, 200, 1, 1);
			scene.addChild(p);
			p.z = 500;
			
//			p2 = new Plane(1, 250, 250, 4, 4);
//			scene.addChild(p2);
//			p2.x = 200;
			
//			lib.buffer.position = p.verticesPointer;
//			for (var i:int = 0; i < 10; i ++)
//			{
//				trace(lib.buffer.readDouble());
//			}
			
			//stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
			addEventListener(Event.ENTER_FRAME, onRenderTick);
		}
		
		protected function onKeyDown(e:KeyboardEvent):void
		{
			viewport.render();
		}
		
		protected function onRenderTick(e:Event):void
		{
//			p.rotationX ++;
//			p.rotationY ++;
//			p.rotationZ ++;
			
//			p2.rotationX --;
//			p2.rotationY --;
//			p2.rotationZ --;
			
			viewport.render();
		}
	}
}