package AlchemyLib.tools
{
	public class Debug
	{
		public static function error(comment:String):void
		{
			throw new Error(comment);
		}
		
		public static function assert(x:Boolean, comment:String):void
		{
			if(!x)
				error(comment);
		}
		
		public static function warning(comment:String):void
		{
			trace(comment);
		}
		
		public static function log(comment:String):void
		{
			trace(comment);
		}
		
		public function Debug()
		{
		}
	}
}