package cn.niuniuzhu.preloader.drawer
{
	import flash.display.Sprite;
	
	internal interface IDrawer
	{
		function set precent(value:Number):void;
		function get precent():Number;
		function get drawer():Sprite
		function get drawerWidth():Number
		function get drawerHeight():Number
		function stop():void
	}
}