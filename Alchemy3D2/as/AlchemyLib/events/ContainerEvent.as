package AlchemyLib.events
{
	import AlchemyLib.container.Entity;
	import AlchemyLib.container.SceneContainer;
	
	import flash.events.Event;

	public class ContainerEvent extends Event
	{
		public static const ADDED_TO_SCENE:String = "added to scene";
		public static const ADDED_TO_ENTITY:String = "added to entity";
		
		public static const REMOVED_FROM_SCENE:String = "removed frome scene";
		public static const REMOVED_FROM_ENTITY:String = "removed from entity";
		
		public function get container():SceneContainer
		{
			return _container;
		}
		
		public function ContainerEvent(type:String, container:SceneContainer, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			
			_container = container;
		}
		
		
		private var _container:SceneContainer;
	}
}