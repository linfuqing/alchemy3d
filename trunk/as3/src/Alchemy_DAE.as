package
{
	import cn.alchemy3d.device.Device;
	import cn.alchemy3d.objects.DAE;

	[SWF(width="600",height="400",backgroundColor="#000000",frameRate="60")]
	public class Alchemy_DAE extends Device
	{
		public function Alchemy_DAE()
		{
			super();
			
			var dae:DAE = new DAE();
		}
	}
}