package AlchemyLib.base
{
	public class Node extends Pointer
	{
		public function get node():uint
		{
			return _node;
		}
		
		protected var _node:uint = NULL;
		
		public function Node()
		{
			super();
		}
		
	}
}