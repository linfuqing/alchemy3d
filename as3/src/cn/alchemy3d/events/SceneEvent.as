package cn.alchemy3d.events
{
	import cn.alchemy3d.container.Scene3D;
	
	import flash.events.Event;

	public class SceneEvent extends Event
	{
		public static const ADDED_TO_SCENE:String = "added to scene";
		
		public function get node():Scene3D
		{
			return scene;
		}
		
		public function SceneEvent(type:String, scene:Scene3D, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			
			this.scene = scene;
		}
		
		
		private var scene:Scene3D;
	}
}