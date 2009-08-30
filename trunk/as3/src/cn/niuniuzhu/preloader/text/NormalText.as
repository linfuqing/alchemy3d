package cn.niuniuzhu.preloader.text
{
	import cn.niuniuzhu.preloader.Preloader;
	
	import flash.events.Event;
	import flash.text.TextFormat;

	public class NormalText extends AbstractText
	{
		public function NormalText(drawingBoard:Preloader, userInsetFont:Boolean, font:String, size:int, color:uint, center:Boolean = true, bold:Boolean=false, italic:Boolean=false, underline:Boolean=false)
		{
			_userInsetFont = userInsetFont;
			
			super(drawingBoard, new TextFormat(font, size, color, bold, italic, underline));
			
			if (center)
			{
				text.x = (_drawingBoard.width - text.textWidth) * .5;
				text.y = (_drawingBoard.height - text.textHeight) * .5;
			}
		}
		
		override protected function updateView(e:Event = null):void
		{
			super.updateView(e);
		}
	}
}