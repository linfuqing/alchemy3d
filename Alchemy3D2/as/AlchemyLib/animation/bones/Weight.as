package AlchemyLib.animation.bones
{
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Pointer;
	import AlchemyLib.tools.Debug;
	
	import flash.utils.ByteArray;
	
	public class Weight extends Pointer
	{
		public function get weight():Number
		{
			return _weight;
		}
		
		public function set weight(value:Number):void
		{
			_weight = value;
			
			if(_pointer)
			{
				Library.memory.position = _pointer;
				Library.memory.writeFloat(value);
			}
		}
		
		public function get index():uint
		{
			return _index;
		}
		
		public function Weight(weight:Number, index:uint)
		{
			_weight = weight;
			_index  = index;
			super();
		}
		
		static public function serialize(input:Weight, data:ByteArray):void
		{
			if(!input || !data)
			{
				Debug.warning("no input.");
				
				return;
			}
			
			data.writeFloat(input._weight);
			data.writeUnsignedInt(input._index);
		}
		
		static public function unserialize(data:ByteArray):Weight
		{
			if(!data)
			{
				Debug.warning("no data.");
				
				return null;
			}
			
			
			return new Weight( data.readFloat(), data.readUnsignedInt() );
		}
		
		override protected function initialize():void
		{
			_pointer = 0;
		}
		
		internal function setPointer(pointer:uint):void
		{
			_pointer = pointer;
		}
		
		public override function toString():String
		{
			return "weight:" + _weight + ', ' + "index:" + _index;
		}
		
		private var _weight:Number;
		private var _index:uint;
	}
}