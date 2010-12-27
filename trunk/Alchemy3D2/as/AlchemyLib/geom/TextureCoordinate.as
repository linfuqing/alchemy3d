package AlchemyLib.geom
{
	import AlchemyLib.base.Pointer;
	
	import flash.utils.ByteArray;
	
	public class TextureCoordinate extends Pointer
	{
		public function get u():Number
		{
			return _u;
		}
		
		public function get v():Number
		{
			return _v;
		}
		
		public function TextureCoordinate(u:Number, v:Number)
		{
			_u = u;
			_v = v;
			
			super();
		}
		
		static public function serialize(input:TextureCoordinate, data:ByteArray):void
		{
			data.writeFloat(input._u);
			data.writeFloat(input._v);
		}
		
		static public function unserialze(data:ByteArray):TextureCoordinate
		{
			var u:Number;
			var v:Number;
			
			u = data.readFloat();
			v = data.readFloat();
			
			return new TextureCoordinate(u, v);
		}
		
		internal var _u:Number;
		internal var _v:Number;
	}
}