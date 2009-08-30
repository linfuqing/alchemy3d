package cn.niuniuzhu.preloader.text
{
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	internal interface IText
	{
		function set precent(value:Number):void
		function get precent():Number
		function get text():TextField
		function get format():TextFormat
		function get textWidth():Number
		function get textHeight():Number
		function stop():void
	}
}