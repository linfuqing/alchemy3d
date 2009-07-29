package cn.alchemy3d.device
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.Viewport3D;
	
	import flash.display.Sprite;
	import flash.events.Event;

	public class Device extends Sprite
	{
		public var pointer:uint;
		
		public var scenes:Vector.<Scene3D>;
		public var viewports:Vector.<Viewport3D>;
		public var cameras:Vector.<Camera3D>;
		public var lights:Vector.<Light3D>;
		
		protected var lib:Library;
		
		public function Device()
		{
			super();
			
			scenes = new Vector.<Scene3D>();
			viewports = new Vector.<Viewport3D>();
			cameras = new Vector.<Camera3D>();
			lights = new Vector.<Light3D>();
			
			lib = Library.getInstance();
			
			this.pointer = lib.alchemy3DLib.initializeDevice();
		}
		
		public function propertySetting(property:int, value:int):void
		{
			lib.alchemy3DLib.propertySetting(property, value);
		}
		
		public function addViewport(viewport:Viewport3D):void
		{
			viewports.push(viewport);
			addChild(viewport);
			viewport.initialize(this.pointer);
		}
		
		public function addCamera(camera:Camera3D):void
		{
			cameras.push(camera);
			camera.initialize(this.pointer);
		}
		
		public function addScene(scene:Scene3D):void
		{
			scenes.push(scene);
			scene.initialize(this.pointer);
		}
		
		public function addLight(light:Light3D):void
		{
			lights.push(light);
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
			lib.alchemy3DLib.render(this.pointer);
			
			var viewport:Viewport3D;
			
			for each (viewport in this.viewports)
			{
				viewport.render();
			}
		}
	}
}