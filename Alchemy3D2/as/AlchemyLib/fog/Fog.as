package AlchemyLib.fog
{
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Pointer;
	
	import flash.geom.ColorTransform;

	public class Fog extends Pointer
	{
		public static const FOG_EXP:uint = 1;
		public static const FOG_EXP2:uint = 2;
		public static const FOG_LINEAR:uint = 3;
		
		private var _colorPointer:uint;
		private var _startPointer:uint;
		private var _endPointer:uint;
		private var _densityPointer:uint;
		private var _modePointer:uint;
		
		private var _color:ColorTransform;
		private var _start:Number;
		private var _end:Number;
		private var _density:Number
		private var _mode:uint;
		
		public function Fog( color:ColorTransform = null, start:Number = 500, end:Number = 1000, density:Number = 1, mode:uint = 1)
		{
			this._color = color;
			this._start = start;
			this._end = end;
			this._density = density;
			this._mode = mode;
			
			super();
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeFog( _color, _start, _end, _density, _mode );
			
			_pointer			= ps[0];
			_colorPointer		= ps[1];
			_startPointer		= ps[2];
			_endPointer			= ps[3];
			_densityPointer		= ps[4];
			_modePointer		= ps[5];
		}
	}
}