package
{
	import cn.alchemy3d.device.Basic;
	import cn.alchemy3d.objects.DAE;

	[SWF(width="600",height="400",backgroundColor="#000000",frameRate="60")]
	public class Alchemy_DAE extends Basic
	{
		public function Alchemy_DAE()
		{
			super();
			
			var dae:DAE = new DAE();
		}
	}
}