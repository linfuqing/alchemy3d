package cn.alchemy3d.fog
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	
	import flash.geom.ColorTransform;

	public class Fog extends Pointer
	{
		private var _colorPointer:uint;
		private var _minDistPointer:uint;
		private var _maxDistPointer:uint;
		
		private var _color:ColorTransform;
		private var _minDist:Number;
		private var _maxDist:Number;
		
		public function Fog( color:ColorTransform = null, minDist:Number = 100, maxDist:Number = 5000)
		{
			this._color = color;
			this._minDist = minDist;
			this._maxDist = maxDist;
			
			super();
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeFog( _color, _minDist, _maxDist );
			
			_pointer		= ps[0];
			_colorPointer	= ps[1];
			_minDistPointer	= ps[2];
			_maxDistPointer	= ps[3];
		}
	}
}