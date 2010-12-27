package AlchemyLib.events
{
	import flash.events.Event;

	public class LoadEvent extends Event
	{
		/**
		* @eventType progress
		*/
		public static const PROGRESS:String = "progress";
		
		public static const TEXTURE_LOAD_ERROR:String = "textureLoadError";
		
		public static const ANIMATIONS_COMPLETE:String = "animationComplete";
		
		public static const TEXTURE_COMPLETE:String = "textureComplete";
		
		/**
		 * 读取进度
		 */
		public var bytesLoaded:Number = -1;
		
		/**
		 * 总长度
		 */ 
		public var bytesTotal:Number = -1;
		
		/**
		 * 文件名
		 */
		public var info:Object;
		
		public function LoadEvent(type:String, info:Object = null, bytesLoaded:Number=-1, bytesTotal:Number=-1, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			
			this.bytesLoaded = bytesLoaded;
			this.bytesTotal = bytesTotal;
			this.info = info;
		}
	}
}