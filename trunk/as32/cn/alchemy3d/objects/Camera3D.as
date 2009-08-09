package cn.alchemy3d.objects
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;

	public class Camera3D extends Pointer
	{
		private var field_of_view:Number;
		private var near_clip    :Number;
		private var far_clip     :Number;
		
		private var fieldOfViewPointer:uint;
		private var nearClipPointer   :uint;
		private var farClipPointer    :uint;
		private var eyePointer        :uint;
		private var movePointer       :uint;
		
		private var _eye:Object3D;
		
		public function set fieldOfView( value:Number ):void
		{
			field_of_view = value;
			
			Library.instance().buffer.position = fieldOfViewPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			setMove();
		}
		
		public function get fieldOfView():Number
		{
			return field_of_view;
		}
		
		public function set nearClip( value:Number ):void
		{
			near_clip = value;
			
			Library.instance().buffer.position = nearClipPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			setMove();
		}
		
		public function get nearClip():Number
		{
			return near_clip;
		}
		
		public function set farClip( value:Number ):void
		{
			far_clip = value;
			
			Library.instance().buffer.position = farClipPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			setMove();
		}
		
		public function get farClip():Number
		{
			return far_clip;
		}
		
		public function set eye( value:Object3D ):void
		{
			if( value != NULL )
			{
				_eye = value;
				
				Library.instance().buffer.position = eyePointer;
				
				Library.instance().buffer.writeUnsignedInt( value.pointer );
				
				setMove();
			}
		}
		
		public function get eye():Object3D
		{
			return _eye;
		}
		
		public function Camera3D( fieldOfView:Number, nearClip:Number, farClip:Number eye:Object3D )
		{
			field_of_view = fieldOfView;
			near_clip     =    nearClip;
			far_clip      =    farClip;
			
			if( !eye )
			{
				_eye = new Object3D();
			}
			
			super();
		}
		
		override protected function initialize():void
		{
			var camera:Array = Library.instance().methods.initializeCamera( field_of_view, near_clip, far_clip, _eye.pointer );
			
			_pointer           = camera[0];
			
			eyePointer         = camera[1];
			
			fieldOfViewPointer = camera[2];
			
			farClipPointer     = camera[3];
			
			nearClipPointer    = camera[4];
			
			movePointer        = camera[5];
		}
		
		private function setMove():void
		{
			Library.instance().buffer.position = movePointer;
			
			Library.instance().buffer.writeInt( TRUE );
		}
	}
}