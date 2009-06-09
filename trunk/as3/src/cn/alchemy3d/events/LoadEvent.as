package cn.alchemy3d.events
{
	import flash.events.Event;

	public class LoadEvent extends Event
	{
		public static const WORLD_INIT_OK:String = "worldInitOk";
		
		/**
		 * 几何信息建立完毕后派发事件
		* @eventType loadComplete
		*/
		public static const GEOMETRY_BUILD_COMPLETE:String = "geometryLoadComplete";
		
		/**
		 * 文件读取完毕后派发事件
		* @eventType loadComplete
		*/
		public static const TEXTURE_LOAD_COMPLETE:String = "textureLoadComplete";
		
		/**
		 * 文件读取中派发事件
		* @eventType progress
		*/
		public static const PROGRESS:String = "progress";
		
		/**
		 * 文件读取错误时派发事件
		* @eventType loadError
		*/
		public static const LOAD_ERROR:String = "loadError";
		
		/**
		 * 材质读取完毕后派发事件
		* @eventType colladaMaterialsLoadComplete
		*/
		public static const COLLADA_MATERIALS_LOAD_COMPLETE:String = "colladaMaterialsLoadComplete";
		
		public static const DOCUMENT_LOAD_COMPLETE:String = "documentLoadComplete";
		
		/**
		 * 动画读取完毕后派发事件
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
		public var filename:String;
		
		public function LoadEvent(type:String, filename:String = "", bytesLoaded:Number=-1, bytesTotal:Number=-1, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			
			this.bytesLoaded = bytesLoaded;
			this.bytesTotal = bytesTotal;
			this.filename = filename;
		}
	}
}