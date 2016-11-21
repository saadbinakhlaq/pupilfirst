require 'rails_helper'

feature 'MoocStudent Sign In' do
  let!(:first_module) { create :course_module, :with_2_chapters, module_number: 1 }
  let!(:second_module) { create :course_module, :with_2_chapters, module_number: 2 }
  let(:mooc_student) { create :mooc_student }
  let(:first_chapter_name) { first_module.module_chapters.find_by(chapter_number: 1).name }
  let(:university) { create :university }

  context 'User visits the sixways start page' do
    before :each do
      visit six_ways_start_path
    end

    scenario 'User signs up for MOOC' do
      click_link 'Sign-up as Student'

      fill_in 'Name', with: 'John Doe'
      fill_in 'Email', with: 'johndoe@sv.co'
      fill_in 'Mobile number', with: '9876543210'
      choose 'Male'
      fill_in 'mooc_student_signup_university_id', with: university.id
      fill_in 'College', with: 'Doe Learning Centre'
      select 'Graduated', from: 'Semester'
      select 'Kerala', from: 'State'

      click_button 'Sign up'

      expect(page).to have_content('Sign-in link sent!')

      open_email('johndoe@sv.co')

      mooc_student = MoocStudent.last
      expect(current_email.subject).to eq("Welcome to SV.CO's SixWays MOOC")
      expect(current_email.body).to have_text("Thank you for signing up for SV.CO's SixWays MOOC.")
      expect(current_email.body).to have_text("token=#{mooc_student.user.login_token}")
      expect(current_email.body).to have_text(CGI.escape(six_ways_start_path))
    end
  end
end