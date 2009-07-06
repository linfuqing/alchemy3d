package cn.alchemy3d.view
{
	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.scene.Scene3D;
	
	import flash.display.Sprite;
	import flash.events.Event;

	public class Device extends Sprite
	{
		public var pointer:uint;
		
		protected var viewports:Vector.<Viewport3D>;
		
		protected var lib:Alchemy3DLib;
		
		public function Device()
		{
			super();
			
			viewports = new Vector.<Viewport3D>();
			
			lib = Alchemy3DLib.getInstance();
			
			this.pointer = lib.alchemy3DLib.initializeDevice();
		}
		
		public function addViewport(viewport:Viewport3D):Viewport3D
		{
			var pointerArr:Array = lib.alchemy3DLib.initializeViewport(this.pointer, viewport.viewWidth, viewport.viewHeight, viewport.scene.pointer, viewport.camera.pointer);
			viewport.pointer = pointerArr[0];
			viewport.gfxPointer = pointerArr[1];
			
			viewports.push(viewport);
			
			addChild(viewport);
			
			return viewport;
		}
		
		public function addCamera(camera:Camera3D):void
		{
			camera.initializeCamera(this.pointer);
		}
		
		public function addScene(scene:Scene3D):void
		{
			scene.initializeScene(this.pointer);
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