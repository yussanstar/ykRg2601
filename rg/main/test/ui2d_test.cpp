
#include "ui2d_test.h"

namespace ykl::test
{
	class UI2DTestImpl : public UI2DTest
	{
	private:
		IAllocatorPtr m_allocator;

	public:
		UI2DTestImpl(IAllocatorPtr _allocator) :
			m_allocator(_allocator)
		{
		}

	public:
		bool Launch() override
		{
			return true;
		}

		bool IsRequiredExitLoop() override
		{
			return false;
		}

		void Loop() override
		{
			float stepTime = m_time.Stamp();

			//if(stepTime > m_prevTime + 1.0f)
			{
				//m_prevTime = stepTime;

				m_windowMngr->Update();
				m_inputMngr->Update(stepTime);

				Render();
			}
		}

		void Terminate() override
		{
		}

	public:
		void Render()
		{
			auto& device = m_gfxVulkanContext->GetDevice()->GetVkDevice();
			auto& swapchain = m_gfxVulkanSurface->swapchain->GetVkSwapchain();
			auto& presentCompleteSemaphore = m_gfxVulkanSwapchainRenderSet->presentCompleteSemaphore->GetVkSemaphore();
			auto& renderCompleteSemaphore = m_gfxVulkanSwapchainRenderSet->renderCompleteSemaphore->GetVkSemaphore();
			auto& renderPass = m_gfxVulkanSurface->renderPass->GetVkRenderPass();

			// 同期処理
			m_crntSwapchainIndex = 0;
			{
				auto device = m_gfxVulkanContext->GetDevice()->LockVkDevice();
				auto result = device->acquireNextImageKHR(
					swapchain,
					UINT64_MAX,
					presentCompleteSemaphore,
					vk::Fence(nullptr),
					&m_crntSwapchainIndex);
			}

			auto& crntFence = m_gfxVulkanSwapchainRenderSet->fences[m_crntSwapchainIndex]->GetVkFence();
			auto& crntCommandBuffer = m_gfxVulkanSwapchainRenderSet->commandBuffers[m_crntSwapchainIndex]->GetVkCommandBuffer();
			auto& crntFrameBuffer = m_gfxVulkanSurface->frameBuffers[m_crntSwapchainIndex]->GetVkFrameBuffer();

			// フェンス待ち
			{
				auto device = m_gfxVulkanContext->GetDevice()->LockVkDevice();
				auto result = device->waitForFences(1, &crntFence, VK_TRUE, UINT64_MAX);
				ykAssert(result == vk::Result::eSuccess);
			}

			// コマンドバッファ開始
			vk::CommandBufferBeginInfo cmdBufBeginInfo{};
			auto result = crntCommandBuffer.begin(&cmdBufBeginInfo);
			ykAssert(result == vk::Result::eSuccess);

			// クリア値
			yk::STLArray<vk::ClearValue, 2> clearValue;
			float depth[2] = { 0.0f, 0 };
			yk::STLArray<float, 4> clearColor = { 1.0f, 0.2f, 0.2f, 1.0f };
			//yk::STLArray<float, 4> clearColor = { 1.0f, 0.2f, 0.2f + m_crntSwapchainIndex % 2 * 0.5f, 1.0f };
			clearValue[0].setColor(clearColor);
			memcpy(&(clearValue[1]), depth, sizeof(float) * 2);
			auto surfaceExtent = m_gfxVulkanSurface->surface->GetSurfaceExtent2D();

			// レンダーパス開始
			vk::RenderPassBeginInfo renderPassBI{};
			renderPassBI.renderPass = renderPass;
			renderPassBI.framebuffer = crntFrameBuffer;
			renderPassBI.renderArea.offset = VkOffset2D{ 0, 0 };
			renderPassBI.renderArea.extent = surfaceExtent;
			renderPassBI.pClearValues = clearValue.data();
			renderPassBI.clearValueCount = uint32_t(clearValue.size());
			crntCommandBuffer.beginRenderPass(&renderPassBI, vk::SubpassContents::eInline);

			// ビューポート
			vk::Viewport viewport = {
				0.0f, 0.0f,
				(float)surfaceExtent.width, (float)surfaceExtent.height,
				0.0f, 1.0f
			};
			crntCommandBuffer.setViewport(0, 1, &viewport);

			// シザー
			vk::Rect2D scissor;
			scissor.setOffset({ 0,0 });
			scissor.setExtent(surfaceExtent);
			crntCommandBuffer.setScissor(0, 1, &scissor);

			// YKUI描画
			m_inst->PrepareRender();
			m_inst->Render(crntCommandBuffer);

			// レンダーパス終了
			crntCommandBuffer.endRenderPass();

			// コマンドバッファ終了
			crntCommandBuffer.end();

			// フェンスのリセット
			{
				auto device = m_gfxVulkanContext->GetDevice()->LockVkDevice();
				result = device->resetFences(1, &crntFence);
				ykAssert(result == vk::Result::eSuccess);
			}

			// 提出
			vk::SubmitInfo submitInfo{};
			vk::PipelineStageFlags waitStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &crntCommandBuffer;
			submitInfo.pWaitDstStageMask = &waitStageMask;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &renderCompleteSemaphore;

			auto queue = m_gfxVulkanContext->GetDevice()->LockVkQueue();
			result = queue->submit(1, &submitInfo, crntFence);
			ykAssert(result == vk::Result::eSuccess);

			// Present
			vk::PresentInfoKHR presentInfo{};
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &swapchain;
			presentInfo.pImageIndices = &m_crntSwapchainIndex;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &renderCompleteSemaphore;

			result = queue->presentKHR(&presentInfo);
			ykAssert(result == vk::Result::eSuccess);
		}

	private:
		void Test_NodeConnection()
		{
			auto nodeA = m_inst->CreateNode<ykui::Node>();
			auto nodeB = m_inst->CreateNode<ykui::Node>();
			auto nodeC = m_inst->CreateNode<ykui::Node>();
			auto nodeD = m_inst->CreateNode<ykui::Node>();
			auto nodeE = m_inst->CreateNode<ykui::Node>();

			ykAssert(nodeA && nodeB && nodeC && nodeD);

			ykui::Node::Connect(nodeA, nodeB);
			ykAssert(nodeA->GetChildNum() == 1);
			ykAssert(nodeA->GetChild(0) == nodeB);
			ykAssert(nodeB->GetParent() == nodeA);

			ykui::Node::Connect(nodeA, nodeC);
			ykAssert(nodeA->GetChildNum() == 2);
			ykAssert(nodeA->GetChild(0) == nodeB);
			ykAssert(nodeA->GetChild(1) == nodeC);
			ykAssert(nodeB->GetParent() == nodeA);
			ykAssert(nodeC->GetParent() == nodeA);

			ykui::Node::Connect(nodeB, nodeD);
			ykAssert(nodeA->GetChildNum() == 2);
			ykAssert(nodeA->GetChild(0) == nodeB);
			ykAssert(nodeA->GetChild(1) == nodeC);
			ykAssert(nodeB->GetChildNum() == 1);
			ykAssert(nodeB->GetChild(0) == nodeD);
			ykAssert(nodeB->GetParent() == nodeA);
			ykAssert(nodeC->GetParent() == nodeA);
			ykAssert(nodeD->GetParent() == nodeB);

			ykui::Node::Connect(nodeA, nodeE);
			ykAssert(nodeA->GetChildNum() == 3);
			ykAssert(nodeA->GetChild(0) == nodeB);
			ykAssert(nodeA->GetChild(1) == nodeC);
			ykAssert(nodeA->GetChild(2) == nodeE);

			nodeA->RemoveChild(nodeC);
			ykAssert(nodeA->GetChildNum() == 2);
			ykAssert(nodeA->GetChild(0) == nodeB);
			ykAssert(nodeA->GetChild(1) == nodeE);
			ykAssert(!nodeC->GetParent());

			nodeE->RemoveFromParent();
			ykAssert(nodeA->GetChildNum() == 1);
			ykAssert(nodeA->GetChild(0) == nodeB);
			ykAssert(!nodeE->GetParent());

			ykui::Node::Connect(nodeA, nodeD);
			ykAssert(nodeA->GetChildNum() == 2);
			ykAssert(nodeA->GetChild(0) == nodeB);
			ykAssert(nodeA->GetChild(1) == nodeD);
			ykAssert(nodeD->GetParent() == nodeA);
			ykAssert(nodeB->GetChildNum() == 0);

			nodeA->RemoveAllChildren();
			ykAssert(nodeA->GetChildNum() == 0);
			ykAssert(!nodeB->GetParent());
			ykAssert(!nodeC->GetParent());
			ykAssert(!nodeD->GetParent());
			ykAssert(!nodeE->GetParent());
		}

		void Test_PrepareNodes()
		{
			node[0] = m_inst->CreateNode<ykui::RectNode>();
			node[0]->SetLocation({ {0.1f, ykui::LocationType::Ratio }, {0.1f, ykui::LocationType::Ratio} });
			node[0]->SetSize({ {0.8f, ykui::LocationType::Ratio }, {0.8f, ykui::LocationType::Ratio} });
			node[0]->SetRot(0);
			node[0]->SetAnchor({ 0.0f, 0.0f });
			node[0]->SetIsFill(true);
			node[0]->SetFillColor(ykui::Color4::Cyan());
			node[0]->SetIsEdge(true);
			node[0]->SetEdgeColor(ykui::Color4::Black());
			node[0]->SetEdgeWidth(3);
			node[0]->SetZ(0.0f);

			node[1] = m_inst->CreateNode<ykui::RectNode>();
			node[1]->SetLocation({ {0.1f, ykui::LocationType::Ratio }, {0.1f, ykui::LocationType::Ratio} });
			node[1]->SetSize({ {0.8f, ykui::LocationType::Ratio }, {0.8f, ykui::LocationType::Ratio} });
			node[1]->SetAnchor({ 0.0f, 0.0f });
			node[1]->SetIsFill(true);
			node[1]->SetFillColor(ykui::Color4::Green());
			node[1]->SetIsEdge(false);
			node[1]->SetEdgeColor(ykui::Color4::Black());
			node[1]->SetEdgeWidth(1);
			node[1]->SetZ(10.0f);
			ykui::Node::Connect(node[0], node[1]);

			image[0] = m_inst->CreateImage();
			image[0]->Load("../texture/sample_image.png");
			image[1] = m_inst->CreateImage();
			image[1]->Load("../texture/sample_image_alpha.png");

			imgNode[0] = m_inst->CreateNode<ykui::ImageNode>();
			imgNode[0]->SetLocation({ {0.1f, ykui::LocationType::Ratio }, {0.1f, ykui::LocationType::Ratio} });
			imgNode[0]->SetSize({ {0.8f, ykui::LocationType::Ratio }, {0.8f, ykui::LocationType::Ratio} });
			imgNode[0]->SetAnchor({ 0.0f, 0.0f });
			imgNode[0]->SetIsFill(true);
			imgNode[0]->SetFillColor(ykui::Color4::White());
			imgNode[0]->SetIsEdge(false);
			imgNode[0]->SetEdgeColor(ykui::Color4::Black());
			imgNode[0]->SetEdgeWidth(1);
			imgNode[0]->SetZ(20.0f);
			imgNode[0]->SetImage(image[0]);
			ykui::Node::Connect(node[1], imgNode[0]);

			imgNode[1] = m_inst->CreateNode<ykui::ImageNode>();
			imgNode[1]->SetLocation({ {50.0f, ykui::LocationType::Raw }, {50.0f, ykui::LocationType::Raw} });
			imgNode[1]->SetSize({ {80.0f, ykui::LocationType::Raw }, {80.0f, ykui::LocationType::Raw} });
			imgNode[1]->SetAnchor({ 0.5f, 0.5f });
			imgNode[1]->SetIsFill(true);
			imgNode[1]->SetFillColor(ykui::Color4::White());
			imgNode[1]->SetIsEdge(true);
			imgNode[1]->SetEdgeColor(ykui::Color4::Black());
			imgNode[1]->SetEdgeWidth(1);
			imgNode[1]->SetZ(30.0f);
			imgNode[1]->SetImage(image[1]);
			ykui::Node::Connect(node[1], imgNode[1]);
		}
	};

	std::unique_ptr<YKUITest> YKUITest::Create()
	{
		return std::make_unique<YKUITestImpl>(yk::DefaultMemID);
	}
}
#endif