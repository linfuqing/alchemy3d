package cn.alchemy3d.animation.channel
{
	import cn.alchemy3d.objects.Entity;	

	/**
	 * @author Tim Knip
	 */ 
	public class MatrixChannel3D extends AbstractChannel3D
	{
		public var member:String;
		
		/**
		 * Constructor.
		 * 
		 * @param	target
		 * @param	name
		 */ 
		public function MatrixChannel3D(target:Entity, name:String=null)
		{
			super(target, name);
			this.member = null;
		}
		
		/**
		 * Updates this channel.
		 * 
		 * @param	keyframe
		 * @param	target
		 */ 
		public override function updateToFrame(keyframe:uint):void
		{
			super.updateToFrame(keyframe);	
			
//			target.copyTransformFromMatrix(currentKeyFrame.output[0]);
		}
		
		public override function updateToTime(time:Number):void
		{
			super.updateToTime(time);
			
//			target.copyTransformFromMatrix(currentKeyFrame.output[0]);
		}
	}
}