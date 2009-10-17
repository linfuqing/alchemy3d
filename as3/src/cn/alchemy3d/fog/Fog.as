package cn.alchemy3d.fog
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	
	import flash.geom.ColorTransform;

	public class Fog extends Pointer
	{
		private var _colorPointer:uint;
		private var _distancePointer:uint;
		private var _depthPointer:uint;
		
		private var _color:ColorTransform;
		private var _distance:Number;
		private var _depth:Number;
		
		public function Fog( color:ColorTransform = null, distance:Number = 100, depth:Number = 5000)
		{
			this._color = color;
			this._distance = distance;
			this._depth = depth;
			
			super();
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeFog( _color, _distance, _depth );
			
			_pointer			= ps[0];
			_colorPointer		= ps[1];
			_distancePointer	= ps[2];
			_depthPointer		= ps[3];
		}
	}
}