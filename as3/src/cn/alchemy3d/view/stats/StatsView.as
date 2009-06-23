package cn.rc3dx.view.stats
{
	import cn.niuniuzhu.rc3d.render.RenderEngine;
	
	import flash.events.Event;
	import flash.system.System;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;

	public class StatsView extends AbstractStatsView
	{
		private var statsFormat:TextFormat;
		private var render:RenderEngine;
		
		private var memInfoTestField:TextField;
		private var fpsInfoTextField:TextField;
		private var triInfoTextField:TextField;
		
		public function StatsView(render:RenderEngine)
		{
			super();
			
			this.render = render;
			
			init();
		}
		
		protected function init():void
		{
			setupView();
		}
		
		protected function setupView():void
		{
			opaqueBackground = 0;
			
			statsFormat = new TextFormat("Arial", 12, 0xFFFFFF, false, false, false);
			
			fpsInfoTextField = new TextField();
			fpsInfoTextField.y = 0;
			fpsInfoTextField.autoSize = TextFieldAutoSize.LEFT;
			fpsInfoTextField.defaultTextFormat = statsFormat;
			addChild(fpsInfoTextField);
			
			memInfoTestField = new TextField();
			memInfoTestField.y = 18;
			memInfoTestField.autoSize = TextFieldAutoSize.LEFT;
			memInfoTestField.defaultTextFormat = statsFormat;
			addChild(memInfoTestField);
			
			triInfoTextField = new TextField();
			triInfoTextField.y = 36;
			triInfoTextField.autoSize = TextFieldAutoSize.LEFT;
			triInfoTextField.defaultTextFormat = statsFormat;
			addChild(triInfoTextField);
		}
		
		override protected function onFrame(event:Event):void
		{
			super.onFrame(event);
			
			memInfoTestField.text = "Mem : "+ (System.totalMemory / 1048576).toFixed(2) + "MB";
			fpsInfoTextField.text = "FPS : "+ fps;
			triInfoTextField.text = "Faces : " + render.data.renderItemNumber;
			fpsInfoTextField.setTextFormat(statsFormat);
		}
	
	}
}