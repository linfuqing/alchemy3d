package cn.alchemy3d.tools
{
	public class Alchemy3DLog
	{
		static public function warning(str:String):void
		{
			trace("WARNING: " + str);
		}
		
		static public function error(str:String):void
		{
			throw new Error(str);
		}
	}
}