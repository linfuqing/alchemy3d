package AlchemyLib.animation
{
	import AlchemyLib.base.Library;
	import AlchemyLib.container.Entity;
	import AlchemyLib.geom.Mesh3D;
	import AlchemyLib.tools.Debug;
	
	import flash.utils.ByteArray;
	
	public class AnimationObject extends Entity
	{
		public static const TIME_MODE_GROBAL  :int = 0;
		public static const TIME_MODE_NORMAL  :int = 1;
		public static const TIME_MODE_ACCURATE:int = 2;
		
		public function set minTime(time:int):void
		{
			Library.memory.position = minTimePointer;
			Library.memory.writeInt(time);
		}
		
		public function get minTime():int
		{
			Library.memory.position = minTimePointer;
			return Library.memory.readInt();
		}
		
		public function set maxTime(time:int):void
		{
			Library.memory.position = maxTimePointer;
			Library.memory.writeInt(time);
		}
		
		public function get maxTime():int
		{
			Library.memory.position = maxTimePointer;
			return Library.memory.readInt();
		}
		
		public function set time(value:int):void
		{
			Library.memory.position = timePointer;
			Library.memory.writeInt(value);
		}
		
		public function get time():int
		{
			Library.memory.position = timePointer;
			return Library.memory.readInt();
		}
		
		public function set timeMode(value:int):void
		{
			Library.memory.position = timeModePointer;
			
			Library.memory.writeInt(value);
		}
		
		public function get timeMode():int
		{
			Library.memory.position = timeModePointer;
			
			return Library.memory.readInt();
		}
		
		public function set defaultTransform(transform:Array):void
		{
			_defaultTransform = transform;
		}
		
		public function get defaultTransform():Array
		{
			return _defaultTransform;
		}
		
		public function AnimationObject(autoPlay:Boolean = true, loop:Boolean = true)
		{
			super();
			
			if(autoPlay)
				play(loop);
			else
				stop();
		}
		
		public override function destroy(all:Boolean):void
		{
			Library.alchemy3DLib.destroyAnimation(animationPointer);
			
			super.destroy(all);
		}
		
		public function play(loop:Boolean = true):void
		{
			if(!isPlayPointer)
				return;
			
			Library.memory.position = loopPointer;
			
			Library.memory.writeInt( loop ? TRUE : FALSE );
			
			Library.memory.position = isPlayPointer;
			
			Library.memory.writeInt( TRUE );
		}
		
		public function pause():void
		{
			if(!isPlayPointer)
				return;
			
			Library.memory.position = isPlayPointer;
			
			Library.memory.writeInt( FALSE );
		}
		
		public function stop():void
		{
			pause();
			
			if(_defaultTransform)
			{
				var object:Entity;
				for(var info:String in _defaultTransform)
				{
					object = getChild(info);
					
					if(object)
						object.transform = _defaultTransform[info];
				}
			}
		}
		
		public function setupSkeletalAnimation(animation:Vector.<SkeletalAnimation>):void
		{
			if(!animation)
			{
				Debug.warning("no animation");
				
				return;
			}
			
			var length:uint = animation.length;
			var bone:Entity;
			var maxTime:int = int.MIN_VALUE;
			var minTime:int = int.MAX_VALUE;
			
			for(var i:uint = 0; i < length; i ++)
			{
				bone = getChild(animation[i].name);
				
				if(bone && bone.bone)
				{
					bone.skeletalAnimation = animation[i];
					
					maxTime = Math.max(animation[i]._times[animation[i]._times.length - 1] * 1000, maxTime);
					minTime = Math.min(animation[i]._times[0                             ] * 1000, minTime);
				}
			}
			
			this.minTime = minTime;
			this.maxTime = maxTime;
		}
		
		override protected function initialize():void
		{
			super.initialize();
			
			var ps:Array = Library.alchemy3DLib.initializeAnimation(_pointer, TRUE, TRUE, int.MIN_VALUE, int.MAX_VALUE);
			
			animationPointer    = ps[0];
			isPlayPointer       = ps[1];
			loopPointer         = ps[2];
			minTimePointer      = ps[3];
			maxTimePointer      = ps[4];
			durationTimePointer = ps[5];
			timePointer         = ps[6];
			timeModePointer     = ps[7];
		}
		
		internal function getAnimationPointer():uint
		{
			return animationPointer;
		}
		
		protected var _defaultTransform:Array;
		
		private var animationPointer:uint;
		private var isPlayPointer:uint;
		private var loopPointer:uint;
		private var minTimePointer:uint;
		private var maxTimePointer:uint;
		private var durationTimePointer:uint;
		private var timeModePointer:uint;
		private var timePointer:uint;
	}
}