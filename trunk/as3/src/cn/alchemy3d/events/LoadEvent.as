package cn.alchemy3d.events
{
	import flash.events.Event;

	public class LoadEvent extends Event
	{
		/**
		 * 几何信息建立完毕后派发事件
		* @eventType loadComplete
		*/
		public static const GEOMETRY_BUILD_COMPLETE:String = "geometryLoadComplete";
		
		/**
		* @eventType loadComplete
		*/
		public static const TEXTURE_READY:String = "textureReady";
		
		/**
		* @eventType progress
		*/
		public static const PROGRESS:String = "progress";
		
		/**
		* @eventType loadError
		*/
		public static const LOAD_ERROR:String = "loadError";
		
		/**
		* @eventType colladaMaterialsLoadComplete
		*/
		public static const ANIMATIONS_COMPLETE:String = "animationComplete";
		
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