package cn.alchemy3d.view
{
	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.lib.Alchemy3DLib;
	
	import flash.display.DisplayObject;
	import flash.display.Sprite;
	import flash.events.Event;

	public class Screen2D extends Sprite
	{
		public var pointer:uint;
		
		protected var viewports:Vector.<View3D>;
		
		protected var lib:Alchemy3DLib;
		
		public function Screen2D()
		{
			super();
			
			viewports = new Vector.<View3D>();
			
			lib = Alchemy3DLib.getInstance();
			
			this.pointer = lib.alchemy3DLib.initializeScreen();
		}
		
		override public function addChild(child:DisplayObject):DisplayObject
		{
			if (child is View3D)
			{
				lib.alchemy3DLib.addViewport(this.pointer, View3D(child).pointer);
				
				viewports.push(child);
			}
			
			return super.addChild(child);
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
			
			var viewport:View3D;
			
			for each (viewport in this.viewports)
			{
				viewport.render();
			}
		}
	}
}