package cn.alchemy3d.base
{
	public class Instance extends Pointer
	{
		protected var instance:Library;
		
		public function Instance()
		{
			super();
			instance = Library.getInstance();
		}

	}
}