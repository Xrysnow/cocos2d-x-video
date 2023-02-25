#pragma once
#include "cocos2d.h"
#include "VideoDecoder.h"
#include <functional>

namespace video
{
	class Player : public cocos2d::Sprite
	{
	public:
		enum class PlayMode
		{
			/** step a const time per frame */
			STEP,
			/** realtime time -> video time, not recommended */
			REALTIME,
			/** game frame -> video frame */
			FRAME,
			/** invoke update manually */
			MANUAL,
		};
		static Player* create(const std::string& path);
	protected:
		bool init_(const std::string& path);
	public:
		void vplay();
		void vstop();
		void vpause();
		void vresume();
		/** can only seek key frame, not recommended to set a non-0 value */
		void seek(uint32_t frame);
		/** step dt in decoder and get data, will step 1 frame if dt<0 */
		void update(float dt) override;

		/** see PlayMode */
		void setPlayMode(PlayMode m);
		/** used for PlayMode::STEP mode */
		void setStep(double v) { step = v; }
		/** auto invoke removeFromParentAndCleanup when finished */
		void setAutoRemove(bool b) { autoRemove = b; }
		/** set callback when finished */
		void setVideoEndCallback(const std::function<void()>& func);
		/** save current frame to file */
		void saveCurrentFrame(const std::string& path);

		Decoder* getDecoder() const { return decoder; }

	protected:
		void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
		void playerSeekTime(double sec);
		Decoder* decoder = nullptr;
		uint8_t* vbuf = nullptr;

		double currentTime = 0.0;
		double step = 1.0 / 60;
		bool autoUpdate = false;
		bool autoRemove = false;
		bool isPlaying = false;
		bool texureDirty = false;
		PlayMode mode = PlayMode::STEP;

		std::function<void()> videoEndCallback;

		Player();
		~Player() override;
	};
}
