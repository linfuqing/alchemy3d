package cn.alchemy3d.device
{
	public interface IDevice
	{
		function initialize(devicePointer:uint):void;
		function allotPtr(ps:Array):void;
	}
}