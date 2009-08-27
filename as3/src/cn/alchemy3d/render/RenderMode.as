package cn.alchemy3d.render
{
	public class RenderMode
	{
		//定义渲染模式常量
		/**
		 * 不渲染
		 */
		public static const RENDER_NONE:uint												= 0x000000;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 纹理映射
		 */
		public static const RENDER_TEXTRUED_TRIANGLE_INVZB_32:uint							= 0x000002;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 双线性纹理
		 */
		public static const RENDER_TEXTRUED_BILERP_TRIANGLE_INVZB_32:uint					= 0x000004;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + Flat光照模型 + 纹理映射
		 */
		public static const RENDER_TEXTRUED_TRIANGLE_FSINVZB_32:uint						= 0x000008;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + Gouraud光照模型 + 纹理映射
		 */
		public static const RENDER_TEXTRUED_TRIANGLE_GSINVZB_32:uint						= 0x000016;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + Flat光照模型
		 */
		public static const RENDER_FLAT_TRIANGLE_INVZB_32:uint								= 0x000032;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + Gouraud光照模型
		 */
		public static const RENDER_GOURAUD_TRIANGLE_INVZB_32:uint							= 0x000064;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 透视矫正 + 纹理映射
		 */
		public static const RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32:uint				= 0x000128;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 透视矫正 + Flat光照模型 + 纹理映射
		 */
		public static const RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_INVZB_32:uint			= 0x000256;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 透视矫正 + Gouraud光照模型 + 纹理映射
		 */
		public static const RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FSINVZB_32:uint			= 0x001024;
	}
}